#include "stdafx.h"
#include "..\Public\WhirlWind_Hit_Effect.h"
#include "GameInstance.h"

CWhirlWind_Hit_Effect::CWhirlWind_Hit_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CWhirlWind_Hit_Effect::CWhirlWind_Hit_Effect(const CWhirlWind_Hit_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CWhirlWind_Hit_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind_Hit_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector  vPos;

		memcpy(&vPos, pArg, sizeof(_vector));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	WINDDATA* pWind = new WINDDATA;

	m_vecWind.push_back(pWind);

	for(_int i = 0; i < 4; i++)
	{
		SMOKEDATA* pSmoke = new SMOKEDATA;

		pSmoke->iIndex = rand() % 16;

		m_vecSmoke.push_back(pSmoke);
	}

	return S_OK;
}

void CWhirlWind_Hit_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;

	
	if (XMVectorGetX(m_vFlashScale) >= 2.f)
	{
		m_vFlashScale -= m_vFlashScaleOrigin * 2.f * fTimeDelta;
	}
	else
	{
		m_vFlashScale += XMVectorSet(20.f, 0.f, 0.f, 0.f) * fTimeDelta;
		m_vFlashScaleOrigin = m_vFlashScale;
	}

	for (auto& pData : m_vecWind)
	{
		pData->fAlpha -= 2.f * fTimeDelta;
		pData->vPos += XMVectorSet(0.f, 1.f, 0.f, 0.f) * fTimeDelta;
	}

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 6.f * fTimeDelta);

	m_fSmokeScale += 1.f * fTimeDelta;
	m_fSmokeAlpha -= 2.f * fTimeDelta;



	if (m_fLiveTime > 1.f)
		m_bDead = true;

}

void CWhirlWind_Hit_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWhirlWind_Hit_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if(m_fLiveTime < 0.4f)
		Render_Flash();

	Render_Wind();

	Render_Smoke();

	return S_OK;
}

HRESULT CWhirlWind_Hit_Effect::Render_Wind()
{
	for (auto& pWind : m_vecWind)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix		WorldMatrix;
		WorldMatrix =
			XMMatrixIdentity() *
			XMMatrixScaling(pWind->fScale, pWind->fScale, pWind->fScale) *
			XMMatrixTranslation(0.f, XMVectorGetY(pWind->vPos), 0.f)*
			m_pTransformCom->Get_WorldMatrix();

		m_pShaderCom_Model->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_float4x4));

		m_pShaderCom_Model->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom_Model->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureWind->Bind_OnShader(m_pShaderCom_Model, "g_AlphaTexture");

		_float2 MoveTex = _float2(-0.f, 0.f);

		m_pShaderCom_Model->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

		m_pShaderCom_Model->Set_RawValue("g_Alpha", &pWind->fAlpha, sizeof(_float));

		m_pShaderCom_Model->Set_RawValue("g_vColor", &XMVectorSet(0.5f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom_Model->Begin(2);

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CWhirlWind_Hit_Effect::Render_Smoke()
{
	for (auto& pSmoke : m_vecSmoke)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix		WorldMat;
		_matrix		ViewMat;

		WorldMat =
			XMMatrixIdentity() *
			XMMatrixScaling(m_fSmokeScale, m_fSmokeScale, 1.f) *
			m_pTransformCom->Get_WorldMatrix();

		_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
		_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
		_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

		ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
		ViewMat = XMMatrixInverse(nullptr, ViewMat);

		WorldMat.r[0] = ViewMat.r[0] * xScale;
		WorldMat.r[1] = ViewMat.r[1] * yScale;
		WorldMat.r[2] = ViewMat.r[2] * zScale;

		m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));

		m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureSmoke->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture", pSmoke->iIndex);

		m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fSmokeAlpha, sizeof(_float));

		m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom_Rect->Begin(2);

		m_pRectCom->Render();
	}

	return S_OK;
}

HRESULT CWhirlWind_Hit_Effect::Render_Flash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * 
		XMMatrixScaling(XMVectorGetX(m_vFlashScale), XMVectorGetY(m_vFlashScale), 1.f) * 
		m_pTransformCom->Get_WorldMatrix();
	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;

	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureFlash->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fFlashAlpha, sizeof(_float));

	m_pShaderCom_Rect->Begin(1);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CWhirlWind_Hit_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom_Model)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;


	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Tornado"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Tornado_Alpha"), TEXT("Com_Texture_Wind"), (CComponent**)&m_pTextureWind)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Tornado_Smoke"), TEXT("Com_Texture_Smoke"), (CComponent**)&m_pTextureSmoke)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Tornado_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTextureFlash)))
		return E_FAIL;
}

HRESULT CWhirlWind_Hit_Effect::SetUp_ConstantTable(_uint iNumModel)
{
	return S_OK;
}

CWhirlWind_Hit_Effect * CWhirlWind_Hit_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CWhirlWind_Hit_Effect*		pInstance = new CWhirlWind_Hit_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CWhirlWind_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWhirlWind_Hit_Effect::Clone(void * pArg)
{
	CWhirlWind_Hit_Effect*		pInstance = new CWhirlWind_Hit_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CWhirlWind_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWhirlWind_Hit_Effect::Free()
{
	__super::Free();


	for (auto& pWind : m_vecWind)
	{
		Safe_Delete(pWind);
	}

	m_vecWind.clear();

	for (auto& pSmoke : m_vecSmoke)
	{
		Safe_Delete(pSmoke);
	}

	m_vecSmoke.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRectCom);
	Safe_Release(m_pTextureWind);
	Safe_Release(m_pTextureSmoke);
	Safe_Release(m_pShaderCom_Rect);
	Safe_Release(m_pShaderCom_Model);
}
