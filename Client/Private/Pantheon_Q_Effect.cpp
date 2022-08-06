#include "stdafx.h"
#include "..\Public\Pantheon_Q_Effect.h"
#include "GameInstance.h"
#include "Player.h"

CPantheon_Q_Effect::CPantheon_Q_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_Q_Effect::CPantheon_Q_Effect(const CPantheon_Q_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_Q_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Q_Effect::NativeConstruct(void * pArg)
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
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));
		m_pTransformCom->Go_Right(1.5f);
	}

	m_SourColor = XMVectorSet(1.f, 1.f, 200.f / 255.f, 1.f);
	m_DestColor = XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f);



	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_Q_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;
	if (m_fLiveTime >= 0.3f)
		m_bDead = true;


	if (m_fLiveTime >= 0.1f)
	{
		if(m_fScaleY >= 0.f)
			m_fScaleY -= 10.f * fTimeDelta;
	}

	else
		m_pTransformCom->Go_Right(20.f * fTimeDelta);
;
	m_fTrailAlpha -= 3.f * fTimeDelta;
}

void CPantheon_Q_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_Q_Effect::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Spear();

	Render_Trail();

	return S_OK;
}

HRESULT CPantheon_Q_Effect::Render_Spear()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() *	XMMatrixScaling(m_fScaleX, m_fScaleY, 1.f) * m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	_float fAlpha = 1.f;

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pTextureSpear->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 200.f/ 255.f, 1.f), sizeof(_vector));

	m_pShaderCom_Rect->Begin(2);

	m_pRectCom->Render();

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(2.f * m_fScaleX, 2.f * m_fScaleY, 1.f) * m_pTransformCom->Get_WorldMatrix();
	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));

	m_pTextureSpearBlur->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pShaderCom_Rect->Begin(2);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPantheon_Q_Effect::Render_Trail()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(4.f, 1.f, 1.f) * 
		m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fTrailAlpha, sizeof(_float));

	m_pTextureTrail->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorLerp(m_SourColor, m_DestColor, m_fLiveTime / 0.3f), sizeof(_vector));

	m_pShaderCom_Rect->Begin(2);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPantheon_Q_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear"), TEXT("Com_Texture_Spear"), (CComponent**)&m_pTextureSpear)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear_Blur"), TEXT("Com_Texture_Spear_Blur"), (CComponent**)&m_pTextureSpearBlur)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Trail"), TEXT("Com_Texture_Q_Trail"), (CComponent**)&m_pTextureTrail)))
		return E_FAIL;
}

CPantheon_Q_Effect * CPantheon_Q_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_Q_Effect*		pInstance = new CPantheon_Q_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_Q_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_Q_Effect::Clone(void * pArg)
{
	CPantheon_Q_Effect*		pInstance = new CPantheon_Q_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_Q_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_Q_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRectCom);
	Safe_Release(m_pTextureSpear);
	Safe_Release(m_pTextureSpearBlur);
	Safe_Release(m_pTextureTrail);
	Safe_Release(m_pShaderCom_Rect);
}
