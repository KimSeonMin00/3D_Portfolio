#include "stdafx.h"
#include "..\Public\Player_E_Effect.h"
#include "GameInstance.h"

CPlayer_E_Effect::CPlayer_E_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_E_Effect::CPlayer_E_Effect(const CPlayer_E_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_E_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_E_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector vPos;
	memcpy(&vPos, pArg, sizeof(_vector));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	TRANSFORMALPHA* pTA = new TRANSFORMALPHA;

	pTA->iIndex = rand() % 4;
	pTA->fScale = 0.5f;

	m_vecTARing.push_back(pTA);

	return S_OK;
}

void CPlayer_E_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fScale -= 8.f*fTimeDelta;
	m_fAlpha -= 2.f*fTimeDelta;
	m_fAddTime += fTimeDelta;

	if (m_fAddTime >= 0.2f)
	{
		TRANSFORMALPHA* pTA = new TRANSFORMALPHA;

		pTA->iIndex = rand() % 4;
		pTA->fScale = 0.5f;

		m_vecTARing.push_back(pTA);

		m_fAddTime = 0.f;
	}

	for (auto& pTA : m_vecTARing)
	{
		pTA->fScale += 6.f*fTimeDelta;
	}

	if (m_fAlpha <= 0.f)
		m_bDead = true;
}

void CPlayer_E_Effect::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_E_Effect::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	Render_Flash();

	Render_Ring();

	return S_OK;
}

HRESULT CPlayer_E_Effect::Render_Flash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(3.f, m_fScale, 1.f) * m_pTransformCom->Get_WorldMatrix();

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

	m_pTexture_Flash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pFlash_Buffer->Render();

	return S_OK;
}

HRESULT CPlayer_E_Effect::Render_Ring()
{
	for (_int i = 0; i < m_vecTARing.size(); i++)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		WorldMat = XMMatrixIdentity() * XMMatrixScaling(m_vecTARing[i]->fScale, m_vecTARing[i]->fScale, 1.f) * m_pTransformCom->Get_WorldMatrix();

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

		m_pTexture_Ring->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_vecTARing[i]->iIndex);

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		m_pRing_Buffer->Render();
	}

	return S_OK;
}

HRESULT CPlayer_E_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Flash"), (CComponent**)&m_pFlash_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Ring"), (CComponent**)&m_pRing_Buffer)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Dash_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTexture_Flash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Yasuo_E_Ring"), TEXT("Com_Texture_Ring"), (CComponent**)&m_pTexture_Ring)))
		return E_FAIL;
}

HRESULT CPlayer_E_Effect::SetUp_ConstantTable()
{
	return S_OK;
}

CPlayer_E_Effect * CPlayer_E_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_E_Effect*		pInstance = new CPlayer_E_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_E_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_E_Effect::Clone(void * pArg)
{
	CPlayer_E_Effect*		pInstance = new  CPlayer_E_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_E_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_E_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTexture_Flash);
	Safe_Release(m_pTexture_Ring);
	Safe_Release(m_pFlash_Buffer);
	Safe_Release(m_pRing_Buffer);
	Safe_Release(m_pShaderCom);
}
