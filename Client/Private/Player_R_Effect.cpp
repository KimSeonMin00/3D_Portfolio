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

	for (_int i = 0; i < 4; i++)
	{
		TRANSFORMALPHA* pString = new TRANSFORMALPHA;

		pString->vPos = XMVectorSet(rand()%5 * 0.1f, 5.f , rand() % 5 * 0.1f, 1.f);
		pString->fScale = XMVectorSet(0.5f, 4.f, 1.f, 0.f);

		m_vecString.push_back(pString);
	}

	for (_int i = 0; i < 4; i++)
	{
		TRANSFORMALPHA* pSmoke = new TRANSFORMALPHA;

		pSmoke->iIndex = rand() % 4;
		m_vecSmoke.push_back(pSmoke);
	}

	return S_OK;
}

void CPlayer_R_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	for (auto iter = m_vecString.begin(); iter != m_vecString.end();)
	{
		(*iter)->vPos -= XMVectorSet(0.f, 20.f * fTimeDelta, 0.f, 0.f);

		if (XMVectorGetY((*iter)->vPos) < 0.f)
		{
			Safe_Delete(*iter);
			iter = m_vecString.erase(iter);
			m_bStringFall = true;
		}
		else
			iter++;
	}

	if (m_bStringFall == true)
	{
		m_fAddTime += fTimeDelta;
		if (m_fAddTime > 0.2f)
		{
			TRANSFORMALPHA* pTransformAlpha = new TRANSFORMALPHA;

			pTransformAlpha->vTextureMove.x = m_fTexMoveX;
			m_vecTransformAlpha.push_back(pTransformAlpha);
			m_fTexMoveX += 0.25f;


			if (m_vecRing.size() < 3)
			{
				TRANSFORMALPHA* pRing = new TRANSFORMALPHA;

				pRing->fAlpha = 1.f;
				pRing->fScale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
				m_vecRing.push_back(pRing);
			}

			m_fAddTime = 0.f;
		}

		for (auto iter = m_vecTransformAlpha.begin(); iter != m_vecTransformAlpha.end();)
		{
			(*iter)->vTextureMove.y += 2.f * fTimeDelta;

			iter++;
		}

		for (auto iter = m_vecRing.begin(); iter != m_vecRing.end();)
		{
			(*iter)->fScale += XMVectorSet(9.f * fTimeDelta, 9.f * fTimeDelta, 9.f * fTimeDelta, 0.f);

			if(XMVectorGetX((*iter)->fScale) >= 3.f)
				(*iter)->fAlpha -= 3.f*fTimeDelta;

			iter++;
		}

		m_fAlpha -= 1.f * fTimeDelta;

		if (m_vecRing.size() >= 3)
		{
			if (m_vecRing[2]->fAlpha < 0.f)
			{
				m_fCrackAlpha -= 1.f * fTimeDelta;
				if(m_fCrackAlpha <= 0.f)
					m_bDead = true;
			}
		}

		for (auto& pSmoke : m_vecSmoke)
		{
			pSmoke->fScale += XMVectorSet(2.f, 2.f, 2.f, 0.f)* fTimeDelta;
		}
	}
}

void CPlayer_R_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_R_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_String();

	if (m_bStringFall == true)
	{
		Render_Crack();

		Render_Wind();

		Render_Smoke();

		Render_Flash();	
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::Render_Flash()
{	
	for (_int i = 0; i < m_vecRing.size(); i++)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		WorldMat = XMMatrixIdentity() * XMMatrixScaling(XMVectorGetX(m_vecRing[i]->fScale), XMVectorGetX(m_vecRing[i]->fScale), 1.f) * m_pTransformCom->Get_WorldMatrix();
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
		
		if (i == 0)
			m_pTextureSpark->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");
		else
			m_pTexture_R_Ring->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");
		
		m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_vecRing[i]->fAlpha, sizeof(_float));

		m_pShaderCom_Rect->Begin(2);

		m_pRect_Spark->Render();
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::Render_Wind()
{
	for (_int i = 0; i < m_vecTransformAlpha.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable(0)))
			return E_FAIL;

		m_pTexture->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Set_RawValue("g_vMoveTex", &m_vecTransformAlpha[i]->vTextureMove, sizeof(_float2));

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::Render_String()
{
	for (_int i = 0; i < m_vecString.size(); i++)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;

		_float3 vPos;
		XMStoreFloat3(&vPos, m_vecString[i]->vPos);

		WorldMat = XMMatrixIdentity() * XMMatrixScaling(1.f, 4.f, 1.f) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z) * m_pTransformCom->Get_WorldMatrix();

		m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
		m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureString->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

		m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pShaderCom_Rect->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom_Rect->Begin(0);

		m_pRect_String->Render();
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::Render_Crack()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(2.f, 2.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.2f, 0.f) * m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTexture_Crack->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fCrackAlpha, sizeof(_float));

	m_pShaderCom_Rect->Begin(0);

	m_pRect_Crack->Render();

	return S_OK;
}

HRESULT CPlayer_R_Effect::Render_Smoke()
{
	for (_int i = 0; i < m_vecSmoke.size(); i++)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		_float3 vPos;
		XMStoreFloat3(&vPos, m_vecSmoke[i]->vPos);

		WorldMat = XMMatrixIdentity() * 
			XMMatrixScaling(XMVectorGetX(m_vecSmoke[i]->fScale), XMVectorGetX(m_vecSmoke[i]->fScale), 1.f) *
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

		m_pTexture_Smoke->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

		m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom_Rect->Begin(1);

		m_pRect_String->Render();
	}

	return S_OK;
}

HRESULT CPlayer_R_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Spark)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_String"), (CComponent**)&m_pRect_String)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_Crack"), (CComponent**)&m_pRect_Crack)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Player_R_Blast"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	//Texture
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_Blast"), TEXT("Com_Texture_R_Blast"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Dash_Flash"), TEXT("Com_Texture_R_Spark"), (CComponent**)&m_pTextureSpark)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_String"), TEXT("Com_Texture_R_String"), (CComponent**)&m_pTextureString)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_Ring"), TEXT("Com_Texture_R_Ring"), (CComponent**)&m_pTexture_R_Ring)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_Crack"), TEXT("Com_Texture_R_Crack"), (CComponent**)&m_pTexture_Crack)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Smoke"), TEXT("Com_Texture_R_Smoke"), (CComponent**)&m_pTexture_Smoke)))
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

	for (auto& TransformAlpha : m_vecTransformAlpha)
	{
		Safe_Delete(TransformAlpha);
	}

	m_vecTransformAlpha.clear();

	for (auto& TransformAlpha : m_vecString)
	{
		Safe_Delete(TransformAlpha);
	}

	m_vecString.clear();

	for (auto& TransformAlpha : m_vecRing)
	{
		Safe_Delete(TransformAlpha);
	}

	m_vecRing.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRect_String);
	Safe_Release(m_pRect_Spark);
	Safe_Release(m_pRect_Crack);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTextureSpark);
	Safe_Release(m_pTextureString);
	Safe_Release(m_pTexture_R_Ring);
	Safe_Release(m_pTexture_Crack);
	Safe_Release(m_pTexture_Smoke);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Rect);
}
