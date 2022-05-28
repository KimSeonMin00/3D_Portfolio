#include "..\Public\Camera.h"
#include "Pipeline.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext),
	m_pPipeline(CPipeline::Get_Instance())
{
	Safe_AddRef(m_pPipeline);
}

CCamera::CCamera(const CCamera & rhs)
	:CGameObject(rhs),
	m_pPipeline(rhs.m_pPipeline)
{
	Safe_AddRef(m_pPipeline);
}

HRESULT CCamera::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));

	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
}

void CCamera::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_TransformMatrices()
{
	if (nullptr == m_pPipeline)
		return E_FAIL;

	m_pPipeline->Set_Transform(CPipeline::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix()));

	m_pPipeline->Set_Transform(CPipeline::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeline);
}
