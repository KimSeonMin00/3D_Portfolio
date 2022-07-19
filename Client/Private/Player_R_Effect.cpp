#include "stdafx.h"
#include "..\Public\Player_R_Effect.h"
#include "GameInstance.h"

CPlayer_R_Effect::CPlayer_R_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_R_Effect::CPlayer_R_Effect(const CPlayer_R_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_R_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_R_Effect::NativeConstruct(void * pArg)
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

void CPlayer_R_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAddTime += fTimeDelta;
	if (m_fAddTime > 0.2f)
	{
		TRANSFORMALPHA* pTransformAlpha = new TRANSFORMALPHA;

		pTransformAlpha->vTextureMove.x = m_fTexMoveX;
		m_vecTransformAlpha.push_back(pTransformAlpha);
		m_fTexMoveX += 0.25f;

		m_fAddTime = 0.f;
	}


	for (auto iter = m_vecTransformAlpha.begin(); iter != m_vecTransformAlpha.end();)
	{
		(*iter)->vTextureMove.y += 2.f * fTimeDelta;	

		iter++;
	}

	m_fAlpha -= 0.5f * fTimeDelta;

	if (m_fAlpha < 0.f)
		m_bDead = true;
}

void CPlayer_R_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_R_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (_int i = 0; i < m_vecTransformAlpha.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable(0)))
			return E_FAIL;

		m_pTexture->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Set_RawValue("g_vMoveTex", &m_vecTransformAlpha[i]->vTextureMove, sizeof(_float2));

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_R_Blast"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_R_Blast"), TEXT("Com_Texture_R_Blast"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_R_Effect::SetUp_ConstantTable(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPlayer_R_Effect * CPlayer_R_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_R_Effect*		pInstance = new CPlayer_R_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CWhirlWind_EQ"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_R_Effect::Clone(void * pArg)
{
	CPlayer_R_Effect*		pInstance = new CPlayer_R_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CWhirlWind_EQ"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_R_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTexture);
	Safe_Release(m_pShaderCom);
}
