#include "stdafx.h"
#include "..\Public\Voli_Q_Footprint.h"
#include "GameInstance.h"

CVoli_Q_Footprint::CVoli_Q_Footprint(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CVoli_Q_Footprint::CVoli_Q_Footprint(const CVoli_Q_Footprint & rhs)
	:CGameObject(rhs)
{
}

HRESULT CVoli_Q_Footprint::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_Q_Footprint::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	vPos = XMVectorSetY(vPos, 0.2f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scaled(XMVectorSet(2.f, 2.f, 1.f, 0.f));
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));

	for (_int i = 0; i < 3; i++)
	{
		TARDATA* pTar = new TARDATA;

		pTar->vOffset = XMVectorSet(_float(rand() % 10) * 0.4f - 2.f, 0.f, _float(rand() % 10) * 0.4f - 2.f, 1.f);
		_float fDegree = _float(rand() % 6 * 60);
		pTar->fRadian = XMConvertToRadians(fDegree);

		m_vecTar.push_back(pTar);
	}


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CVoli_Q_Footprint::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAddTime += fTimeDelta;
	m_fLiveTime += fTimeDelta;

	if (m_fLiveTime < 1.f)
	{
		if (m_fAddTime >= 0.2f)
		{
			SPARKDATA* pSpark = new SPARKDATA;

			pSpark->iIndex = rand() % 4;

			_float fDegree = _float(rand() % 6 * 60);
			pSpark->fRadian = XMConvertToRadians(fDegree);

			m_vecSpark.push_back(pSpark);
			m_fAddTime = 0.f;
		}
	}

	for (auto& iter = m_vecSpark.begin(); iter != m_vecSpark.end();)
	{
		if ((*iter)->fScale < 2.f)
			(*iter)->fScale += 6.f * fTimeDelta;

		else
			(*iter)->fAlpha -= 4.f * fTimeDelta;


		if ((*iter)->fAlpha <= 0.f)
		{
			Safe_Delete(*iter);
			iter = m_vecSpark.erase(iter);
		}

		else
			iter++;
	}

	for (auto& iter = m_vecTar.begin(); iter != m_vecTar.end();)
	{
		(*iter)->fTime += fTimeDelta;

		if ((*iter)->fTime >= 0.1f)
		{
			(*iter)->iIndex++;
			(*iter)->fTime = 0.f;

			if ((*iter)->iIndex == 9)
			{
				Safe_Delete(*iter);
				iter = m_vecTar.erase(iter);
				continue;
			}
		}

		iter++;
	}

	if (m_fLiveTime >= 1.f && m_vecSpark.size() <= 0)
	{
		m_bDead = true;
	}
}

void CVoli_Q_Footprint::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_Q_Footprint::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Spark();

	Render_Tar();

	return S_OK;
}

HRESULT CVoli_Q_Footprint::Render_Spark()
{
	for (auto&pSpark : m_vecSpark)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		WorldMat = XMMatrixIdentity() *
			XMMatrixScaling(pSpark->fScale, pSpark->fScale, 1.f) *
			XMMatrixRotationZ(pSpark->fRadian) *
			m_pTransformCom->Get_WorldMatrix();
		_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
		_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
		_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureSpark->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", pSpark->iIndex);
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &pSpark->fAlpha, sizeof(_float));

		m_pShaderCom->Begin(6);

		m_pRect_Buffer->Render();
	}

	return S_OK;
}

HRESULT CVoli_Q_Footprint::Render_Tar()
{
	for (auto&pTar : m_vecTar)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		WorldMat = XMMatrixIdentity() * XMMatrixScaling(0.5f, 0.5f, 1.f) * m_pTransformCom->Get_WorldMatrix();
		_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
		_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
		_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

		ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
		ViewMat = XMMatrixInverse(nullptr, ViewMat);
		ViewMat *= XMMatrixRotationAxis(ViewMat.r[2], pTar->fRadian);

		WorldMat.r[0] = ViewMat.r[0] * xScale;
		WorldMat.r[1] = ViewMat.r[1] * yScale;
		WorldMat.r[2] = ViewMat.r[2] * zScale;

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

		RELEASE_INSTANCE(CGameInstance);

		m_pTextureTar->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", pTar->iIndex);
		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		_float fAlpha = 1.f;

		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		m_pRect_Buffer->Render();
	}

	return S_OK;
}

HRESULT CVoli_Q_Footprint::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Passive_Spark"), TEXT("Com_Texture_Spark"), (CComponent**)&m_pTextureSpark)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Passive_Tar"), TEXT("Com_Texture_Tar"), (CComponent**)&m_pTextureTar)))
		return E_FAIL;

	return S_OK;
}

CVoli_Q_Footprint * CVoli_Q_Footprint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_Q_Footprint*		pInstance = new CVoli_Q_Footprint(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_Q_Footprint"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_Q_Footprint::Clone(void * pArg)
{
	CVoli_Q_Footprint*		pInstance = new CVoli_Q_Footprint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_Q_Footprint"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVoli_Q_Footprint::Free()
{
	__super::Free();

	for (auto& pSpark : m_vecSpark)
	{
		Safe_Delete(pSpark);
	}

	m_vecSpark.clear();

	for (auto& pTar : m_vecTar)
	{
		Safe_Delete(pTar);
	}

	m_vecTar.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureSpark);
	Safe_Release(m_pTextureTar);
	Safe_Release(m_pShaderCom);
}
