#include "stdafx.h"
#include "..\Public\Voli_Hit_Effect.h"
#include "GameInstance.h"

CVoli_Hit_Effect::CVoli_Hit_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CVoli_Hit_Effect::CVoli_Hit_Effect(const CVoli_Hit_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CVoli_Hit_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_Hit_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	vPos = XMVectorSetY(vPos, 0.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CVoli_Hit_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAlpha -= 2.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
		m_bDead = true;
}

void CVoli_Hit_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_Hit_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(3.f, 3.f, 1.f) * m_pTransformCom->Get_WorldMatrix();
	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);
	ViewMat *= XMMatrixRotationAxis(ViewMat.r[2], XMConvertToRadians(_float(rand()% 360)));

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureSpark->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", rand()%4);
	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CVoli_Hit_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Passive_Spark"), TEXT("Com_Texture_Spark"), (CComponent**)&m_pTextureSpark)))
		return E_FAIL;

	return S_OK;
}

CVoli_Hit_Effect * CVoli_Hit_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_Hit_Effect*		pInstance = new CVoli_Hit_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_Hit_Effect::Clone(void * pArg)
{
	CVoli_Hit_Effect*		pInstance = new CVoli_Hit_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

void CVoli_Hit_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureSpark);
	Safe_Release(m_pShaderCom);
}
