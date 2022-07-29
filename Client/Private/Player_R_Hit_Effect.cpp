#include "stdafx.h"
#include "..\Public\Player_R_Hit_Effect.h"
#include "GameInstance.h"

CPlayer_R_Hit_Effect::CPlayer_R_Hit_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_R_Hit_Effect::CPlayer_R_Hit_Effect(const CPlayer_R_Hit_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_R_Hit_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_vSlashScale = XMVectorSet(0.5f, 0.2f, 1.f, 0.f);
	m_fSlashRadian = XMConvertToRadians(rand() % 360);
	m_fSlashAlpha = 1.f;

	for (_int i = 0; i < 20; i++)
	{
		BLOODDATA* pBloodData = new BLOODDATA;

		pBloodData->iIndex = rand() % 4;
		pBloodData->fScale = _float(rand() % 4 + 1) * 0.2f;
		_float fDegree = rand() % 360;
		pBloodData->fRadian = XMConvertToRadians(fDegree);
		pBloodData->fSpeed = _float(rand() % 10 + 10) * 0.2f;
		pBloodData->vDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMMatrixRotationZ(XMConvertToRadians(fDegree)));

		m_vecBloodData.push_back(pBloodData);
	}

	return S_OK;
}

void CPlayer_R_Hit_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;
	//Slash
	m_vSlashScale += XMVectorSet(40.f, 0.f, 0.f, 0.f) * fTimeDelta;
	if (XMVectorGetX(m_vSlashScale) >= 2.f)
		m_fSlashAlpha -= 4.f * fTimeDelta;

	m_fFlashScale -= 10.f * fTimeDelta;
	m_fFlashAlpha -= 5.f *fTimeDelta;

	for (auto& pBloodData : m_vecBloodData)
	{
		pBloodData->vPos += pBloodData->vDir * pBloodData->fSpeed * fTimeDelta;

		if (pBloodData->fScale > 0.f)
			pBloodData->fScale -= 1.f * fTimeDelta;

		else
			pBloodData->fScale = 0.f;
		/*if (m_fScale > 0.f)
		pTA->fScale -= 0.01f * fTimeDelta;
		else
		pTA->fScale = 0.f;*/
	}

	if (m_fLiveTime >= 1.f)
		m_bDead = true;


}

void CPlayer_R_Hit_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_R_Hit_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Flash();

	Render_Slash();

	Render_Blood();

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::Render_Slash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(XMVectorGetX(m_vSlashScale), XMVectorGetY(m_vSlashScale), XMVectorGetZ(m_vSlashScale)) * m_pTransformCom->Get_WorldMatrix();
	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);
	ViewMat *= XMMatrixRotationAxis(ViewMat.r[2], m_fSlashRadian);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureSlash->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fSlashAlpha, sizeof(_float));

	m_pShaderCom->Begin(1);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::Render_Flash()
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

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fFlashAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::Render_Blood()
{
	for (auto& pTA : m_vecBloodData)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;
		_float3 vPos;

		XMStoreFloat3(&vPos, pTA->vPos);

		WorldMat = XMMatrixIdentity()
			* XMMatrixScaling(pTA->fScale, pTA->fScale, 1.f)
			* XMMatrixTranslation(vPos.x, vPos.y, vPos.z)
			* m_pTransformCom->Get_WorldMatrix();

		_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
		_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
		_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

		ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
		ViewMat = XMMatrixInverse(nullptr, ViewMat);
		ViewMat *= XMMatrixRotationAxis(ViewMat.r[2], pTA->fRadian);

		WorldMat.r[0] = ViewMat.r[0] * xScale;
		WorldMat.r[1] = ViewMat.r[1] * yScale;
		WorldMat.r[2] = ViewMat.r[2] * zScale;

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureBlood->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", pTA->iIndex);

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(1);

		m_pRectCom->Render();
	}

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Attack_Slash"), TEXT("Com_Texture_Slash"), (CComponent**)&m_pTextureSlash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_Flash"), TEXT("Com_Texture_Flash"), (CComponent**)&m_pTextureFlash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_R_Blood"), TEXT("Com_Texture_Blood"), (CComponent**)&m_pTextureBlood)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_R_Hit_Effect::SetUp_ConstantTable(_uint iNumModel)
{
	return S_OK;
}

CPlayer_R_Hit_Effect * CPlayer_R_Hit_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_R_Hit_Effect*		pInstance = new CPlayer_R_Hit_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_R_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_R_Hit_Effect::Clone(void * pArg)
{
	CPlayer_R_Hit_Effect*		pInstance = new CPlayer_R_Hit_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_R_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_R_Hit_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRectCom);
	Safe_Release(m_pTextureSlash);
	Safe_Release(m_pTextureFlash);
	Safe_Release(m_pTextureBlood);
	Safe_Release(m_pShaderCom);
}
