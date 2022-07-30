#include "stdafx.h"
#include "..\Public\Hit_Effect_Normal.h"
#include "GameInstance.h"

CHit_Effect_Normal::CHit_Effect_Normal(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CHit_Effect_Normal::CHit_Effect_Normal(const CHit_Effect_Normal & rhs)
	:CGameObject(rhs)
{
}

HRESULT CHit_Effect_Normal::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit_Effect_Normal::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scaled(XMVectorSet(0.1f, 0.1f, 1.f, 0.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CHit_Effect_Normal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fScale += 20.f*fTimeDelta;
	m_pTransformCom->Set_Scaled(XMVectorSet(m_fScale, m_fScale, 1.f, 0.f));

	if (m_fScale >= 2.f)
		m_bDead = true;
}

void CHit_Effect_Normal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CHit_Effect_Normal::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = m_pTransformCom->Get_WorldMatrix();
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

	m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	_float fAlpha = 1.f;
	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CHit_Effect_Normal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	//Texture
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Monster_Hit"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit_Effect_Normal::SetUp_ConstantTable()
{
	return S_OK;
}

CHit_Effect_Normal * CHit_Effect_Normal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CHit_Effect_Normal*		pInstance = new CHit_Effect_Normal(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CHit_Effect_Normal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHit_Effect_Normal::Clone(void * pArg)
{
	CHit_Effect_Normal*		pInstance = new CHit_Effect_Normal(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CHit_Effect_Normal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHit_Effect_Normal::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
