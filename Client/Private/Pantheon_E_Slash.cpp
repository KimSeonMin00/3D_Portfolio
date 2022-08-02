#include "stdafx.h"
#include "..\Public\Pantheon_E_Slash.h"
#include "GameInstance.h"

CPantheon_E_Slash::CPantheon_E_Slash(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_E_Slash::CPantheon_E_Slash(const CPantheon_E_Slash & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_E_Slash::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{

	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_E_Slash::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_matrix  WorldMat;

		memcpy(&WorldMat, pArg, sizeof(_matrix));

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMat.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMat.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMat.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3] + XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Set_Scaled(XMVectorSet(4.f, 0.5f, 1.f, 0.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(-30.f + _float(rand() % 6) * 10.f) / XMConvertToRadians(60.f));
		m_pTransformCom->Go_Right(1.f);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_E_Slash::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;
	if (m_fLiveTime >= 0.2f)
		m_bDead = true;

	m_fScale -= 10.f * fTimeDelta;

	m_pTransformCom->Set_Scaled(XMVectorSet(4.f, m_fScale, 1.f, 0.f));
	m_pTransformCom->Go_Right(20.f * fTimeDelta);
}

void CPantheon_E_Slash::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_E_Slash::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() *	m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	_float fAlpha = 1.f;

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pTextureSlash->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f) , sizeof(_vector));

	m_pShaderCom_Rect->Begin(2);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPantheon_E_Slash::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Slash"), TEXT("Com_Texture_Slash"), (CComponent**)&m_pTextureSlash)))
		return E_FAIL;

	return S_OK;
}

CPantheon_E_Slash * CPantheon_E_Slash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_E_Slash*		pInstance = new CPantheon_E_Slash(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_E_Slash"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_E_Slash::Clone(void * pArg)
{
	CPantheon_E_Slash*		pInstance = new CPantheon_E_Slash(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_E_Slash"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_E_Slash::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRectCom);
	Safe_Release(m_pTextureSlash);
	Safe_Release(m_pShaderCom_Rect);
}
