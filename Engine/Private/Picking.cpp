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

	_float3		RayDir;
	XMStoreFloat3(&RayDir, vMousePos);

	vRayDir = XMVectorSet(RayDir.x, RayDir.y, RayDir.z, 0.f);

	

	_matrix	ViewMatrix;
	ViewMatrix = m_pPipeline->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_vWorldRayDir = XMVector3TransformNormal(vRayDir, ViewMatrix);
	m_vWorldRayPos = XMVector3TransformCoord(vRayPos, ViewMatrix);
}

bool CPicking::Picking(CVIBuffer * pVIBuffer, CTransform * pTransform, _float4 * pOut)
{
	return false;
}

void CPicking::Free()
{
	Safe_Release(m_pPipeline);
}
