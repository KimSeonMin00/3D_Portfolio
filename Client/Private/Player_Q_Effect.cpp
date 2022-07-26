#include "stdafx.h"
#include "..\Public\Player_Q_Effect.h"
#include  "GameInstance.h"

CPlayer_Q_Effect::CPlayer_Q_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_Q_Effect::CPlayer_Q_Effect(const CPlayer_Q_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_Q_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_matrix WorldMat;
	memcpy(&WorldMat, pArg, sizeof(_matrix));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMat.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMat.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMat.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);

	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 4.f, 1.f, 0.f));
	m_pTransformCom->Go_Straight(1.f);
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, 0.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Q_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTime += fTimeDelta;
	if (m_fTime >= 0.1f)
	{
		m_bSword = true;
	}

	if (m_bSword == true)
	{
		m_fScale -= 10.f * fTimeDelta;
		if (m_fScale <= 0.f)
			m_bDead = true;

		m_pTransformCom->Set_Scaled(XMVectorSet(m_fScale, 4.f, 1.f, 0.f));
	}
	
}

void CPlayer_Q_Effect::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_Q_Effect::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (m_bSword == true)
	{
		if (FAILED(m_pTexture_Sword->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;
		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(0);
	}

	else
	{
		if (FAILED(m_pTexture_Indicator->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 0.1f;
 		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);
	}

	m_pVIBuffer_RectCom->Render();

	return S_OK;
}

HRESULT CPlayer_Q_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Yasuo_Q_Indicator"), TEXT("Com_Texture_Indicator"), (CComponent**)&m_pTexture_Indicator)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Yasuo_Q_Sword"), TEXT("Com_Texture_Sword"), (CComponent**)&m_pTexture_Sword)))
		return E_FAIL;
}

HRESULT CPlayer_Q_Effect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}


CPlayer_Q_Effect * CPlayer_Q_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Q_Effect*		pInstance = new CPlayer_Q_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_Q_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Q_Effect::Clone(void * pArg)
{
	CPlayer_Q_Effect*		pInstance = new  CPlayer_Q_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Q_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Q_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTexture_Indicator);
	Safe_Release(m_pTexture_Sword);
	Safe_Release(m_pShaderCom);
}
