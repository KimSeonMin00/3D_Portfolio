#include "stdafx.h"
#include "..\Public\Pantheon_Hit_Effect.h"
#include "GameInstance.h"

CPantheon_Hit_Effect::CPantheon_Hit_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_Hit_Effect::CPantheon_Hit_Effect(const CPantheon_Hit_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_Hit_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Hit_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_Hit_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fFlashScale += 20.f * fTimeDelta;
	m_fFlashAlpha -= 5.f * fTimeDelta;
	m_fSlashScale -= 10.f * fTimeDelta;

	if (m_fFlashAlpha <= 0.f)
		m_bDead = true;
}

void CPantheon_Hit_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_Hit_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Flash();

	Render_Slash();

	return S_OK;
}

HRESULT CPantheon_Hit_Effect::Render_Slash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(2.f * m_fSlashScale, 0.5f * m_fSlashScale, 1.f) * m_pTransformCom->Get_WorldMatrix();
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

	m_pTextureSlash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");
	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	_float fAlpha = 1.f;
	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CPantheon_Hit_Effect::Render_Flash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(m_fFlashScale, m_fFlashScale, 1.f) * m_pTransformCom->Get_WorldMatrix();
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
	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fFlashAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CPantheon_Hit_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Hit_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTextureFlash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Hit_Slash"), TEXT("Com_Texture_Slash"), (CComponent**)&m_pTextureSlash)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPantheon_Hit_Effect::SetUp_ConstantTable()
{
	return S_OK;
}

CPantheon_Hit_Effect * CPantheon_Hit_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_Hit_Effect*		pInstance = new CPantheon_Hit_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_Hit_Effect::Clone(void * pArg)
{
	CPantheon_Hit_Effect*		pInstance = new CPantheon_Hit_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

void CPantheon_Hit_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureFlash);
	Safe_Release(m_pTextureSlash);
	Safe_Release(m_pShaderCom);
}
