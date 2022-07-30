#include "stdafx.h"
#include "..\Public\Player_Hit_UI.h"
#include "GameInstance.h"

CPlayer_Hit_UI::CPlayer_Hit_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CUI(pDevice, pDevice_Context)
{
}

CPlayer_Hit_UI::CPlayer_Hit_UI(const CPlayer_Hit_UI & rhs)
	:CUI(rhs)
{
}

HRESULT CPlayer_Hit_UI::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Hit_UI::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	/* For.Com_Texture */

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Hit_UI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	Set_Pos((_float)(g_iWinCX >> 1), (_float)(g_iWinCY >> 1));
	Set_Scale(_float(g_iWinCX), _float(g_iWinCY));

	return S_OK;
}

void CPlayer_Hit_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAlpha -= 3.f* fTimeDelta;
	if (m_fAlpha <= 0.f)
		m_bDead = true;
}

void CPlayer_Hit_UI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Hit_UI::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CPlayer_Hit_UI * CPlayer_Hit_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Hit_UI*		pInstance = new CPlayer_Hit_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to CPlayer_Hit_UI"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Hit_UI::Clone(void * pArg)
{
	CPlayer_Hit_UI*		pInstance = new CPlayer_Hit_UI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Hit_UI"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Hit_UI::Free()
{
	__super::Free();
}
