#include "stdafx.h"
#include "..\Public\Player_Attack_Effect.h"
#include "GameInstance.h"

CPlayer_Attack_Effect::CPlayer_Attack_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_Attack_Effect::CPlayer_Attack_Effect(const CPlayer_Attack_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_Attack_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Attack_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_SlashTA.fScale = XMVectorSet(0.5f, 0.2f, 1.f, 0.f);
	m_WindTA.fScale = XMVectorSet(0.5f, 0.5f, 1.f, 0.f);

	return S_OK;
}

void CPlayer_Attack_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_SlashTA.fScale += XMVectorSet(40.f, 0.f, 0.f, 0.f) * fTimeDelta;
	if (XMVectorGetX(m_SlashTA.fScale) >= 2.f)
		m_SlashTA.fAlpha -= 8.f * fTimeDelta;

	if (m_bFlash == true)
	{
		m_WindTA.fScale += XMVectorSet(20.f, 20.f, 0.f, 0.f) * fTimeDelta;

		if (XMVectorGetX(m_WindTA.fScale) > 4.f)
		{
			m_bFlash = false;
			m_WindTA.fScale = XMVectorSet(0.1f, 0.1f, 1.f, 0.f);
		}
	}

	else
	{
		m_WindTA.fScale += XMVectorSet(4.f, 4.f, 0.f, 0.f) * fTimeDelta;
		m_WindTA.fAlpha -= 4.f * fTimeDelta;
		if (m_WindTA.fAlpha <= 0.f)
			m_bDead = true;
	}
}

void CPlayer_Attack_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_Attack_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Wind();

	Render_Slash();

	return S_OK;
}

HRESULT CPlayer_Attack_Effect::Render_Slash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(XMVectorGetX(m_SlashTA.fScale), XMVectorGetY(m_SlashTA.fScale), XMVectorGetZ(m_SlashTA.fScale)) * m_pTransformCom->Get_WorldMatrix();
	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTexture_Slash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_SlashTA.fAlpha, sizeof(_float));

	m_pShaderCom->Begin(1);

	m_pSlash_Buffer->Render();

	return S_OK;
}

HRESULT CPlayer_Attack_Effect::Render_Wind()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(XMVectorGetX(m_WindTA.fScale), XMVectorGetY(m_WindTA.fScale), XMVectorGetZ(m_WindTA.fScale)) * m_pTransformCom->Get_WorldMatrix();
	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	if (m_bFlash == true)
	{
		m_pTexture_Flash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));
	}

	else
	{
		m_pTexture_Wind->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 1.f, 1.f, 1.f), sizeof(_vector));
	}

	m_pShaderCom->Set_RawValue("g_Alpha", &m_WindTA.fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pWind_Buffer->Render();

	return S_OK;
}

HRESULT CPlayer_Attack_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_Slash"), (CComponent**)&m_pSlash_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_Wind"), (CComponent**)&m_pWind_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Attack_Slash"), TEXT("Com_Texture_Slash"), (CComponent**)&m_pTexture_Slash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Attack_Wind"), TEXT("Com_Texture_Wind"), (CComponent**)&m_pTexture_Wind)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Attack_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTexture_Flash)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Attack_Effect::SetUp_ConstantTable()
{
	return S_OK;
}

CPlayer_Attack_Effect * CPlayer_Attack_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Attack_Effect*		pInstance = new CPlayer_Attack_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_Attack_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Attack_Effect::Clone(void * pArg)
{
	CPlayer_Attack_Effect*		pInstance = new CPlayer_Attack_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Attack_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Attack_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pSlash_Buffer);
	Safe_Release(m_pWind_Buffer);
	Safe_Release(m_pTexture_Slash);
	Safe_Release(m_pTexture_Wind);
	Safe_Release(m_pShaderCom);
}
