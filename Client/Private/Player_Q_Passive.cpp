#include "stdafx.h"
#include "..\Public\Player_Q_Passive.h"
#include "GameInstance.h"

CPlayer_Q_Passive::CPlayer_Q_Passive(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_Q_Passive::CPlayer_Q_Passive(const CPlayer_Q_Passive & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_Q_Passive::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Passive::NativeConstruct(void * pArg)
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

void CPlayer_Q_Passive::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Q_Passive::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_Q_Passive::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable_Model_1(0)))
		return E_FAIL;

	if (FAILED(m_pTexture_PassiveWind1->Bind_OnShader(m_pShaderCom, "g_AlphaTexture")))
		return E_FAIL;

	_float fAlpha = 1.f;
	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pModelCom_1->Render(0);

	return S_OK;
}

HRESULT CPlayer_Q_Passive::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yasuo_Passive_Q"), TEXT("Com_Model_1"), (CComponent**)&m_pModelCom_1)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yasuo_Passive_Q"), TEXT("Com_Model_2"), (CComponent**)&m_pModelCom_2)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Yasuo_Passive_Q_1"), TEXT("Com_Texture_Passive_Q_1"), (CComponent**)&m_pTexture_PassiveWind1)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Yasuo_Passive_Q_2"), TEXT("Com_Texture_Passive_Q_2"), (CComponent**)&m_pTexture_PassiveWind2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Passive::SetUp_ConstantTable_Model_1(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayer_Q_Passive::SetUp_ConstantTable_Model_2(_uint iNumModel)
{
	return E_NOTIMPL;
}

CPlayer_Q_Passive * CPlayer_Q_Passive::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Q_Passive*		pInstance = new CPlayer_Q_Passive(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_Q_Passive"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Q_Passive::Clone(void * pArg)
{
	CPlayer_Q_Passive*		pInstance = new  CPlayer_Q_Passive(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Q_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Q_Passive::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom_1);
	Safe_Release(m_pModelCom_2);
	Safe_Release(m_pTexture_PassiveWind1);
	Safe_Release(m_pTexture_PassiveWind2);
	Safe_Release(m_pShaderCom);
}
