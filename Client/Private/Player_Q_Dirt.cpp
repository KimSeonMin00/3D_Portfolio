#include "stdafx.h"
#include "..\Public\Player_Q_Dirt.h"
#include "GameInstance.h"

CPlayer_Q_Dirt::CPlayer_Q_Dirt(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_Q_Dirt::CPlayer_Q_Dirt(const CPlayer_Q_Dirt & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_Q_Dirt::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Dirt::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix  WorldMat;

	if (pArg != nullptr)
		memcpy(&WorldMat, pArg, sizeof(_matrix));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);

	for (_int i = 0; i < 10; i++)
	{
		TRANSFORMALPHA* pTA = new TRANSFORMALPHA;

		pTA->iTextureIndex = rand() % 4;
		pTA->fSpeedX = (_float(rand() % 20) + 1.f) * 0.1f;
		pTA->fSpeedY = _float(rand()%3) + 1.f;
		pTA->vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		pTA->vDir = WorldMat.r[2];

		m_vecTA.push_back(pTA);
	}

	for (_int i = 0; i < 5; i++)
	{
		TRANSFORMALPHA* pTARock = new TRANSFORMALPHA;

		pTARock->fSpeedX = _float(rand() % 5) + 1.f;
		pTARock->fSpeedY = _float(rand() % 3) + 1.f;
		pTARock->fScale = _float(rand() % 4 + 1) * 0.1f;
		pTARock->fRadian = XMConvertToRadians(_float(rand() % 180));
		pTARock->vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		pTARock->vDir = XMVector3Normalize(XMVectorSet(_float(rand() % 10), 0.f, _float(rand() % 10), 0.f));
		pTARock->vAxis = XMVector3Normalize(XMVectorSet(_float(rand() % 10 + 1), _float(rand() % 10), _float(rand() % 10), 0.f));
		

		m_vecTARock.push_back(pTARock);
	}

	return S_OK;
}

void CPlayer_Q_Dirt::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto pTA : m_vecTA)
	{
		pTA->vPos += pTA->vDir * pTA->fSpeedX * fTimeDelta + XMVectorSet(0.f, 1.f, 0.f, 0.f) * pTA->fSpeedY * fTimeDelta;
		pTA->fSpeedY -= 3.f *  fTimeDelta;
	}

	for (auto pTARock : m_vecTARock)
	{
		pTARock->vPos += pTARock->vDir * pTARock->fSpeedX * fTimeDelta + XMVectorSet(0.f, 1.f, 0.f, 0.f) * pTARock->fSpeedY * fTimeDelta;
		pTARock->fSpeedY -= 3.f *  fTimeDelta;
		pTARock->fRadian += XMConvertToRadians(30.f * fTimeDelta);
	}

	m_fCrack_Alpha -= 2.f * fTimeDelta;

	m_fLiveTime += fTimeDelta;

	if (m_fLiveTime >= 1.f)
		m_bDead = true;
}

void CPlayer_Q_Dirt::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_Q_Dirt::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (_int i = 0; i < m_vecTA.size(); i++)
	{
		SetUp_ConstantTable(i);

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(1);

		m_pRectCom->Render();
	}

	Render_Rock();

	Render_Crack();

	return S_OK;
}

HRESULT CPlayer_Q_Dirt::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader_NonAnim"), (CComponent**)&m_pModel_ShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Q_Dirt"), TEXT("Com_Texture_Q_Dirt"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	//Texture
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Player_Q_Rock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Q_Crack"), TEXT("Com_Texture_Q_Crack"), (CComponent**)&m_pTextureCrack_Com)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Dirt::SetUp_ConstantTable(_uint iIndex)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_float3 vPos;  
	XMStoreFloat3(&vPos, m_vecTA[iIndex]->vPos);

	WorldMat = XMMatrixIdentity() * XMMatrixTranslation(vPos.x, vPos.y, vPos.z) * m_pTransformCom->Get_WorldMatrix();

	_matrix ViewMat;
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

	m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_vecTA[iIndex]->iTextureIndex);

	return S_OK;
}

HRESULT CPlayer_Q_Dirt::SetUp_ConstantTable_2(_uint iIndex)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_float3 vPos;

	XMStoreFloat3(&vPos, m_vecTARock[iIndex]->vPos);

	WorldMat = XMMatrixIdentity()
		* XMMatrixScaling(m_vecTARock[iIndex]->fScale, m_vecTARock[iIndex]->fScale, m_vecTARock[iIndex]->fScale)
		* XMMatrixRotationAxis(m_vecTARock[iIndex]->vAxis, m_vecTARock[iIndex]->fRadian)
		* XMMatrixTranslation(vPos.x, vPos.y, vPos.z) 
		* m_pTransformCom->Get_WorldMatrix();

	m_pModel_ShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pModel_ShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pModel_ShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_Q_Dirt::Render_Crack()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	_matrix		WorldMat;
	WorldMat = XMMatrixIdentity() * XMMatrixScaling(2.f, 2.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_WorldMatrix();

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(WorldMat));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureCrack_Com->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fCrack_Alpha, sizeof(_float));

	m_pShaderCom->Begin(1);

	m_pRectCom->Render();


	return S_OK;
}

HRESULT CPlayer_Q_Dirt::Render_Rock()
{
	for (_int i = 0; i < m_vecTARock.size(); i++)
	{
		SetUp_ConstantTable_2(i);

		m_pModelCom->SetUp_Material_OnShader(m_pModel_ShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModel_ShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pModel_ShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pModel_ShaderCom->Begin(0);

		m_pModelCom->Render(0);
	}

	return S_OK;
}

CPlayer_Q_Dirt * CPlayer_Q_Dirt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Q_Dirt*		pInstance = new CPlayer_Q_Dirt(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_Q_Dirt"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Q_Dirt::Clone(void * pArg)
{
	CPlayer_Q_Dirt*		pInstance = new CPlayer_Q_Dirt(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Q_Dirt"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Q_Dirt::Free()
{
	__super::Free();

	for (auto& pTA : m_vecTA)
	{
		Safe_Delete(pTA);
	}

	m_vecTA.clear();

	for (auto& pTARock : m_vecTARock)
	{
		Safe_Delete(pTARock);
	}

	m_vecTARock.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRectCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCrack_Com);
	Safe_Release(m_pShaderCom);
}
