#include "stdafx.h"
#include "..\Public\Pantheon_Shield.h"
#include "GameInstance.h"

CPantheon_Shield::CPantheon_Shield(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_Shield::CPantheon_Shield(const CPantheon_Shield & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_Shield::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Shield::NativeConstruct(void * pArg)
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
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3] + XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_pTransformCom->Go_Left(2.f);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_Shield::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPantheon_Shield::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_Shield::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModel_Wing ||
		nullptr == m_pModel_Center)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_float2 MoveTex = _float2(-0.f, 0.f);

	m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	_float fAlpha = 1.f;

	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pModel_Wing->SetUp_Material_OnShader(m_pShaderCom, "g_AlphaTexture", 0, aiTextureType_DIFFUSE);

	m_pShaderCom->Begin(2);

	m_pModel_Wing->Render(0);

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pModel_Center->SetUp_Material_OnShader(m_pShaderCom, "g_AlphaTexture", 0, aiTextureType_DIFFUSE);

	m_pShaderCom->Begin(2);

	m_pModel_Center->Render(0);

	return S_OK;
}

HRESULT CPantheon_Shield::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon_E_Wing"), TEXT("Com_Model_Wing"), (CComponent**)&m_pModel_Wing)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon_E_Center"), TEXT("Com_Model_Center"), (CComponent**)&m_pModel_Center)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Shield::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPantheon_Shield * CPantheon_Shield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_Shield*		pInstance = new CPantheon_Shield(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_Shield"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_Shield::Clone(void * pArg)
{
	CPantheon_Shield*		pInstance = new CPantheon_Shield(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_Shield"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel_Wing);
	Safe_Release(m_pModel_Center);
	Safe_Release(m_pShaderCom);
}
