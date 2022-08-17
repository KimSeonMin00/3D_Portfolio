#include "stdafx.h"
#include "..\Public\Boss_HP.h"
#include "GameInstance.h"

CBoss_HP::CBoss_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CUI(pDevice, pDevice_Context)
{
}

CBoss_HP::CBoss_HP(const CBoss_HP & rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_HP::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_HP::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	/* For.Com_Texture */

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Monster_HP_Frame"), TEXT("Com_Texture_Frame"), (CComponent**)&m_pTexture_Frame)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Monster_HP"), TEXT("Com_Texture_HP"), (CComponent**)&m_pTexture_HP)))
		return E_FAIL;

	Set_Pos((_float)(g_iWinCX >> 1), 50.f);
	Set_Scale(1200.f, 50.f);

	return S_OK;
}

void CBoss_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fRatio <= 0.f)
		m_bDead = true;
}

void CBoss_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBoss_HP::Render()
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

	if (FAILED(m_pTexture_Frame->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pTexture_HP->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_Ratio", &m_fRatio, sizeof(_float));

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CBoss_HP * CBoss_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CBoss_HP*		pInstance = new CBoss_HP(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to CBoss_HP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_HP::Clone(void * pArg)
{
	CBoss_HP*		pInstance = new CBoss_HP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CBoss_HP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_HP::Free()
{
	__super::Free();
}
