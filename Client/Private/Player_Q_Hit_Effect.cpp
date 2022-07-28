#include "stdafx.h"
#include "..\Public\Player_Q_Hit_Effect.h"
#include "GameInstance.h"

CPlayer_Q_Hit_Effect::CPlayer_Q_Hit_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_Q_Hit_Effect::CPlayer_Q_Hit_Effect(const CPlayer_Q_Hit_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_Q_Hit_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Hit_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	for (_int i = 0; i < 15; i++)
	{
		TRANSFORMALPHA* pTA = new TRANSFORMALPHA;

		pTA->iIndex = rand() % 4;
		pTA->fScale = _float(rand() % 4 + 1) * 0.1f;
		_float fDegree = rand() % 360;
		pTA->fRadian = XMConvertToRadians(fDegree);
		pTA->fSpeed = _float(rand() % 10 + 10) * 0.1f;
		pTA->vDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMMatrixRotationZ(XMConvertToRadians(fDegree)));
		pTA->vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		m_vecTA.push_back(pTA);
	}

	return S_OK;
}

void CPlayer_Q_Hit_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bFlash == true)
	{
		m_fFlashAlpha += 6.f * fTimeDelta;
		m_fScale -= 12.f * fTimeDelta;

		if (m_fScale <= 0.f)
		{
			m_bFlash = false;
		}
	}

	else
	{
		m_fLiveTime += fTimeDelta;

		for (auto& pTA : m_vecTA)
		{
			pTA->vPos += pTA->vDir * pTA->fSpeed * fTimeDelta * 6.f;

			/*if (m_fScale > 0.f)
				pTA->fScale -= 0.01f * fTimeDelta;
			else
				pTA->fScale = 0.f;*/
		}

		if (m_fLiveTime >= 0.1f)
			m_bDead = true;
	}
}

void CPlayer_Q_Hit_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer_Q_Hit_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_bFlash == true)
		Render_Flash();
	else
		Render_Tar();

	return S_OK;
}

HRESULT CPlayer_Q_Hit_Effect::Render_Flash()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() 
		* XMMatrixScaling(m_fScale, m_fScale, 1.f) 
		* m_pTransformCom->Get_WorldMatrix();

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

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fFlashAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pFlash_Buffer->Render();

	return S_OK;
}

HRESULT CPlayer_Q_Hit_Effect::Render_Tar()
{

	for (auto& pTA : m_vecTA)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;
		_float3 vPos;

		XMStoreFloat3(&vPos, pTA->vPos);

		WorldMat = XMMatrixIdentity()
			* XMMatrixScaling(pTA->fScale * 0.5f , pTA->fScale * 3.f, 1.f)
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

		m_pTexture_Flash_Tar->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", pTA->iIndex);

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(1);

		m_pFlash_Tar_Buffer->Render();
	}

	return S_OK;
}

HRESULT CPlayer_Q_Hit_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_Slash"), (CComponent**)&m_pFlash_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect_Wind"), (CComponent**)&m_pFlash_Tar_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Q_Hit"), TEXT("Com_Texture_Hit"), (CComponent**)&m_pTexture_Flash)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_Q_Hit_Tar"), TEXT("Com_Texture_Tar"), (CComponent**)&m_pTexture_Flash_Tar)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Q_Hit_Effect::SetUp_ConstantTable()
{
	return S_OK;
}

CPlayer_Q_Hit_Effect * CPlayer_Q_Hit_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_Q_Hit_Effect*		pInstance = new CPlayer_Q_Hit_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_Q_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Q_Hit_Effect::Clone(void * pArg)
{
	CPlayer_Q_Hit_Effect*		pInstance = new CPlayer_Q_Hit_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_Q_Hit_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Q_Hit_Effect::Free()
{
	__super::Free();

	for (auto& pTA : m_vecTA)
	{
		Safe_Delete(pTA);
	}

	m_vecTA.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pFlash_Buffer);
	Safe_Release(m_pFlash_Tar_Buffer);
	Safe_Release(m_pTexture_Flash);
	Safe_Release(m_pTexture_Flash_Tar);
	Safe_Release(m_pShaderCom);
}
