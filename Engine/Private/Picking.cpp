#include "..\Public\Picking.h"
#include "Pipeline.h"
#include "Transform.h"
#include "VIBuffer.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
	:m_pPipeline(CPipeline::Get_Instance())
{
	Safe_AddRef(m_pPipeline);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	m_hWnd = hWnd;

	m_iWinCX = iWinCX;
	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Transform_ToWorldSpace()
{
	POINT		ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_vector		vMousePos;

	vMousePos = XMVectorSet(((_float)ptMouse.x / (m_iWinCX >> 1)) - 1, ((_float)ptMouse.y / (m_iWinCY >> 1)) * -1 + 1, 0.f, 1.f);

	_matrix ProjMatrix;
	ProjMatrix = m_pPipeline->Get_TransformMatrix(CPipeline::D3DTS_PROJ);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);
	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrix);

	_vector		vRayDir, vRayPos;
	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);   
	vRayDir = vMousePos - vRayPos;

	_matrix	ViewMatrix;
	ViewMatrix = m_pPipeline->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_vWorldRayDir = XMVector3TransformNormal(vRayDir, ViewMatrix);
	m_vWorldRayPos = XMVector3TransformCoord(vRayPos, ViewMatrix);
}

bool CPicking::Picking(CVIBuffer * pVIBuffer, CTransform * pTransform, _float3 * pOut)
{
	_vector		vRayPos = XMVector3TransformCoord(m_vWorldRayPos, XMMatrixInverse(nullptr, pTransform->Get_WorldMatrix()));
	_vector		vRayDir = XMVector3TransformNormal(m_vWorldRayDir, XMMatrixInverse(nullptr, pTransform->Get_WorldMatrix()));
	vRayDir = XMVector3Normalize(vRayDir);

	const _float3* pVerticesPos = pVIBuffer->Get_VerticesPos();

	void* pIndices = pVIBuffer->Get_Indices();

	_uint	iNumFaces = pVIBuffer->Get_NumPrimitive();

	DXGI_FORMAT		eFormat = pVIBuffer->Get_IndexFormat();

	_uint		iIndexSize = eFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;

	for (_uint i = 0; i < iNumFaces; ++i)
	{
		_uint		iIndices[3] = { 0 };

		for (_uint j = 0; j < 3; ++j)
		{
			if (iIndexSize == 2)
				iIndices[j] = *(_ushort*)((((_byte*)pIndices) + (iIndexSize * 3) * i) + iIndexSize * j);
			else
				iIndices[j] = *(_ulong*)((((_byte*)pIndices) + (iIndexSize * 3) * i) + iIndexSize * j);
		}

		_float fDist;

		if (true == TriangleTests::Intersects(
			vRayPos,
			vRayDir,
			XMLoadFloat3(&pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&pVerticesPos[iIndices[2]]),
			fDist
		))
		{
			_vector vPickPos = vRayPos + vRayDir * fDist;

			XMStoreFloat3(pOut, vPickPos);

			return true;
		}
	}

	return false;
}

void CPicking::Free()
{
	Safe_Release(m_pPipeline);
}
