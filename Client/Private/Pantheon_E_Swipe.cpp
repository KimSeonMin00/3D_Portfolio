#include "stdafx.h"
#include "..\Public\Pantheon_E_Swipe.h"
#include "GameInstance.h"

CPantheon_E_Swipe::CPantheon_E_Swipe(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_E_Swipe::CPantheon_E_Swipe(const CPantheon_E_Swipe & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_E_Swipe::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_E_Swipe::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;



	if (pArg != nullptr)
	{
		_matrix  WorldMat;

		memcpy(&WorldMat, pArg, sizeof(_matrix));

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMat.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMat.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMat.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3] + XMVectorSet(0.f, 0.2f, 0.f, 0.f));
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	}

	m_pTransformCom->Set_Scaled(XMVectorSet(10.f, 10.f, 10.f, 0.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_E_Swipe::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;

	if (m_fTexMove < 0.5f)
	{
		m_fAlpha += 8.f * fTimeDelta;
		m_fTexMove += 4.f * fTimeDelta;
	}

	else
		m_fAlpha -= 2.f * fTimeDelta;

	if (m_fLiveTime >= 1.f)
		m_bDead = true;
}

void CPantheon_E_Swipe::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_E_Swipe::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pModel_Swipe)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_float2 MoveTex = _float2(-0.f, 0.f);

	m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 10.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pTexture_Indicator->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

	m_pShaderCom->Begin(2);

	m_pModel_Swipe->Render(0);

	MoveTex = _float2(0.f, -0.5f + m_fTexMove);

	m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 30.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pTexture_Fill->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

	m_pShaderCom->Begin(2);

	m_pModel_Swipe->Render(0);

	return S_OK;
}

HRESULT CPantheon_E_Swipe::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon_E_Swipe"), TEXT("Com_Model_Swipe"), (CComponent**)&m_pModel_Swipe)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Indicator"), TEXT("Com_Texture_Indicator"), (CComponent**)&m_pTexture_Indicator)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Fill"), TEXT("Com_Texture_Fill"), (CComponent**)&m_pTexture_Fill)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_E_Swipe::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPantheon_E_Swipe * CPantheon_E_Swipe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_E_Swipe*		pInstance = new CPantheon_E_Swipe(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_E_Swipe"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_E_Swipe::Clone(void * pArg)
{
	CPantheon_E_Swipe*		pInstance = new CPantheon_E_Swipe(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_E_Swipe"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_E_Swipe::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTexture_Indicator);
	Safe_Release(m_pTexture_Fill);
	Safe_Release(m_pModel_Swipe);
	Safe_Release(m_pRendererCom);
}
