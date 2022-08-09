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

	else if (m_fCastingTime >= 1.3f)
	{
		if (m_fFlashScale > 0.f)
		{
			m_fFlashScale -= 20.f * fTimeDelta;
			m_fFlashAlpha -= 5.f * fTimeDelta;
		}
	}

	else if (m_fCastingTime >= 2.f)
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

	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_fCastingTime >= 1.f)
	{
		Render_Lightening();
	}

	if (m_fCastingTime >= 1.3f)
	{
		if (m_fFlashScale >= 0.f)
		{
			Render_Spark();
		}
	}

	return S_OK;
}

HRESULT CEffect_Voli_E::Render_Spark()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(m_fFlashScale, m_fFlashScale, 1.f) *
		m_pTransformCom->Get_WorldMatrix();

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

	m_pTextureFlash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	_float fAlpha = 1.f;
	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Voli_E::Render_Lightening()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 4.f, 0.f, 0.f);

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(2.f, 8.f, 2.f);

	WorldMat.r[3] = vPos;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureLightening->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.5f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fFlashAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Voli_E::SetUp_Texture_Components()
{
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_E"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTextureFlash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Lightening"), TEXT("Com_Texture_Lightening"), (CComponent**)&m_pTextureLightening)))
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

	Safe_Release(m_pTextureFlash);
	Safe_Release(m_pTextureLightening);
}
