#include "..\Public\Cell.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::NativeConstruct(const _float3 * pPoints, _uint iIndex)
{
	memcpy(&m_vPoint, pPoints, sizeof(_float3) * POINT_END);

	XMStoreFloat3(&m_vLine[LINE_AB], XMLoadFloat3(&m_vPoint[POINT_B]) - XMLoadFloat3(&m_vPoint[POINT_A]));
	XMStoreFloat3(&m_vLine[LINE_BC], XMLoadFloat3(&m_vPoint[POINT_C]) - XMLoadFloat3(&m_vPoint[POINT_B]));
	XMStoreFloat3(&m_vLine[LINE_CA], XMLoadFloat3(&m_vPoint[POINT_A]) - XMLoadFloat3(&m_vPoint[POINT_C]));

	m_iIndex = iIndex;

#ifdef _DEBUG
	if (FAILED(Ready_Debug_Buffer()))
		return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

_bool CCell::Compare_Points(const _float3 * pSourPoint, const _float3 * pDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pSourPoint)))
	{

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pSourPoint)))
	{

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pSourPoint)))
	{

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	return _bool();
}

_bool CCell::isIn(const _float3 * pPosition, _int* pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vSourDir = XMVector3Normalize(XMLoadFloat3(pPosition) - XMLoadFloat3(&m_vPoint[i]));
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.0f, m_vLine[i].x, 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			*pNeighborIndex = m_iNeighborIndex[i];

			return false;
		}
	}
	return true;
}

#ifdef _DEBUG
HRESULT CCell::Ready_Debug_Buffer()
{
	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext, m_vPoint);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CCell::Render(CShader * pShader)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;



	CPipeline*		pPipeline = GET_INSTANCE(CPipeline);

	pShader->Set_RawValue("g_ViewMatrix", &pPipeline->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &pPipeline->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CPipeline);

	pShader->Begin(0);

	m_pVIBuffer->Render();

	return S_OK;
}

#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		MSGBOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif 

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}