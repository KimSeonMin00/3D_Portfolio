#include "stdafx.h"
#include "..\Public\Voli_Passive_Effect.h"
#include "GameInstance.h"

CVoli_Passive_Effect::CVoli_Passive_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CVoli_Passive_Effect::CVoli_Passive_Effect(const CVoli_Passive_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CVoli_Passive_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_Passive_Effect::NativeConstruct(void * pArg)
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

void CVoli_Passive_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAddTime += fTimeDelta;
	if (m_fAddTime >= 0.2f)
	{
		SPARKDATA* pSpark = new SPARKDATA;

		pSpark->iIndex = rand() % 4;

		_float fDegree = _float(rand() % 3 * 30 - 30);
		pSpark->fRadian = XMConvertToRadians(fDegree);

		m_vecSpark.push_back(pSpark);
		m_fAddTime = 0.f;
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
}

void CVoli_Passive_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_Passive_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	for (auto&pSpark : m_vecSpark)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;

		WorldMat = XMMatrixIdentity() * XMMatrixScaling(pSpark->fScale, pSpark->fScale, 1.f) * m_pTransformCom->Get_WorldMatrix();
		_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
		_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
		_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

		ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
		ViewMat = XMMatrixInverse(nullptr, ViewMat);
		ViewMat *= XMMatrixRotationAxis(ViewMat.r[2], pSpark->fRadian);

		WorldMat.r[0] = ViewMat.r[0] * xScale;
		WorldMat.r[1] = ViewMat.r[1] * yScale;
		WorldMat.r[2] = ViewMat.r[2] * zScale;

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

HRESULT CVoli_Passive_Effect::SetUp_Components()
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

	return S_OK;
}

CVoli_Passive_Effect * CVoli_Passive_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_Passive_Effect*		pInstance = new CVoli_Passive_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_Passive_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_Passive_Effect::Clone(void * pArg)
{
	CVoli_Passive_Effect*		pInstance = new CVoli_Passive_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_Passive_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

void CVoli_Passive_Effect::Free()
{
	__super::Free();

	for (auto& pSpark : m_vecSpark)
	{
		Safe_Delete(pSpark);
	}

	m_vecSpark.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureSpark);
	Safe_Release(m_pShaderCom);
}
