#include "stdafx.h"
#include "..\Public\Effect_Voli_E.h"
#include "GameInstance.h"

CEffect_Voli_E::CEffect_Voli_E(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CEffect(pDevice, pDevice_Context)
{
}

CEffect_Voli_E::CEffect_Voli_E(const CEffect & rhs)
	:CEffect(rhs)
{
}

HRESULT CEffect_Voli_E::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Voli_E::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector vPos;
	memcpy(&vPos, pArg, sizeof(_vector));
	vPos = XMVectorSetY(vPos, 0.2f);

	_matrix Rotate = XMMatrixIdentity() * XMMatrixScaling(5.f, 5.f, 1.f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Rotate.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, Rotate.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Rotate.r[2]);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Texture_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Voli_E::Tick(_float fTimeDelta)
{
	m_fLiveTime += fTimeDelta;
	m_fCastingTime += fTimeDelta;

	if (m_fCastingTime <= 0.5f)
	{
		if(m_fAlpha < 1.f)
			m_fAlpha += 2 * fTimeDelta;
	}

	else if (m_fCastingTime >= 2.5f)
	{
		if (m_fAlpha > 0.f)
			m_fAlpha -= 2 * fTimeDelta;
	}



	if (m_fLiveTime >= 3.f)
	{
		m_bDead = true;
	}
}

void CEffect_Voli_E::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Voli_E::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (m_fCastingTime >= 1.3f)
	{
		if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Voli_E::SetUp_Texture_Components()
{
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_E"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
}

CEffect_Voli_E * CEffect_Voli_E::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CEffect_Voli_E*		pInstance = new CEffect_Voli_E(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CEffect_Voli_E"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Voli_E::Clone(void * pArg)
{
	CEffect_Voli_E*		pInstance = new  CEffect_Voli_E(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CEffect_Voli_E"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Voli_E::Free()
{
	__super::Free();
}
