#include "stdafx.h"
#include "..\Public\Pantheon_Q_Spear.h"
#include "GameInstance.h"

CPantheon_Q_Spear::CPantheon_Q_Spear(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_Q_Spear::CPantheon_Q_Spear(const CPantheon_Q_Spear & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_Q_Spear::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Q_Spear::NativeConstruct(void * pArg)
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
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_Q_Spear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;

	m_pTransformCom->Go_Direction(m_pTransformCom->Get_State(CTransform::STATE_UP), 2.f * fTimeDelta);

	if (m_fLiveTime >= 2.f)
		m_bDead = true;
}

void CPantheon_Q_Spear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_Q_Spear::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModel_Spear)

		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_float2 MoveTex = _float2(-0.f, 0.f);

	m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	_float fAlpha = 1.f;

	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pModel_Spear->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", 0, aiTextureType_DIFFUSE);

	m_pShaderCom->Begin(0);

	m_pModel_Spear->Render(0);

	return S_OK;
}

HRESULT CPantheon_Q_Spear::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon_Q_Spear"), TEXT("Com_Model_Spear"), (CComponent**)&m_pModel_Spear)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Q_Spear::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPantheon_Q_Spear * CPantheon_Q_Spear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_Q_Spear*		pInstance = new CPantheon_Q_Spear(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_Q_Spear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_Q_Spear::Clone(void * pArg)
{
	CPantheon_Q_Spear*		pInstance = new CPantheon_Q_Spear(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_Q_Spear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_Q_Spear::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel_Spear);
	Safe_Release(m_pShaderCom);
}
