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
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f) / XMConvertToRadians(60.f));
	m_pTransformCom->Set_Scaled(XMVectorSet(2.f, 2.f, 4.f, 0.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	TRANSFORMALPHA* pTransformAlpha = new TRANSFORMALPHA;

	m_vecTransformAlpha.push_back(pTransformAlpha);

	return S_OK;
}

void CPlayer_Q_Passive::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAddTime += fTimeDelta;
	if (m_fAddTime > 0.3f)
	{
		TRANSFORMALPHA* pTransformAlpha = new TRANSFORMALPHA;

		m_vecTransformAlpha.push_back(pTransformAlpha);

		m_fAddTime = 0.f;
	}


	for (auto iter = m_vecTransformAlpha.begin(); iter != m_vecTransformAlpha.end();)
	{
		(*iter)->vTextureMove.x -= 0.4f * fTimeDelta;

		if ((*iter)->vTextureMove.x <= -0.2f)
		{
			(*iter)->fAlpha -= 1.f * fTimeDelta;
		}

		if ((*iter)->fAlpha < 0.f)
		{
			Safe_Delete(*iter);
			iter = m_vecTransformAlpha.erase(iter);
		}
		else
		{
			iter++;
		}
	}


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vPos = ((CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fInitTime += fTimeDelta;
	if (m_fInitTime >= 0.5f)
	{
		if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
			m_bDead = true;
	}

	RELEASE_INSTANCE(CGameInstance);

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

	/*for (_int i = 0; i < m_vecTransformAlpha.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable_Model_1(0)))
			return E_FAIL;

		if (FAILED(m_pTexture_PassiveWind1->Bind_OnShader(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_Alpha", &m_vecTransformAlpha[i]->fAlpha , sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 1.f, 1.f, 1.f), sizeof(_vector));
		m_pShaderCom->Set_RawValue("g_vMoveTex", &m_vecTransformAlpha[i]->vTextureMove, sizeof(_float2));

		m_pShaderCom->Begin(3);

		m_pModelCom_1->Render(0);

	}*/

	for (_int i = 0; i < m_vecTransformAlpha.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable_Model_2(0)))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_Alpha", &m_vecTransformAlpha[i]->fAlpha, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.5f, 1.f, 1.f, 1.f), sizeof(_vector));
		m_pShaderCom->Set_RawValue("g_vMoveTex", &m_vecTransformAlpha[i]->vTextureMove, sizeof(_float2));

		if (FAILED(m_pTexture_PassiveWind2->Bind_OnShader(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom_2->Render(0);
	}
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

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(-30.f)) * m_pTransformCom->Get_WorldMatrix()), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_Q_Passive::SetUp_ConstantTable_Model_2(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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

	for (auto& TransformAlpha : m_vecTransformAlpha)
	{
		Safe_Delete(TransformAlpha);
	}

	m_vecTransformAlpha.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom_1);
	Safe_Release(m_pModelCom_2);
	Safe_Release(m_pTexture_PassiveWind1);
	Safe_Release(m_pTexture_PassiveWind2);
	Safe_Release(m_pShaderCom);
}
