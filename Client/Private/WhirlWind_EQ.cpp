#include "stdafx.h"
#include "..\Public\WhirlWind_EQ.h"
#include "GameInstance.h"

CWhirlWind_EQ::CWhirlWind_EQ(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CWhirlWind(pDevice, pDevice_Context)
{
}

CWhirlWind_EQ::CWhirlWind_EQ(const CWhirlWind_EQ & rhs)
	:CWhirlWind(rhs)
{
}

HRESULT CWhirlWind_EQ::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind_EQ::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,vPos);
	m_fLiveTime = 0.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	EQDATA* pEqData = new EQDATA;

	pEqData->bTurn = true;
	pEqData->bWhite = false;
	m_iModel++;

	m_vecEqData.push_back(pEqData);

	return S_OK;
}

void CWhirlWind_EQ::Tick(_float fTimeDelta)
{
	m_fLiveTime += fTimeDelta;

	if (m_fLiveTime >= 1.f)
		m_bDead = true;

	m_fAddModelTime += fTimeDelta;
	if (m_fAddModelTime >= 0.1f && m_iModel < 4)
	{
		EQDATA* pEqData = new EQDATA;

		if (m_iModel == 3)
		{
			pEqData->bWhite = false;
		}

		else
		{
			pEqData->bWhite = true;
		}

		m_vecEqData.push_back(pEqData);
		m_iModel++;
		m_fAddModelTime = 0.f;
	}

	for (_int i = 0; i < m_vecEqData.size(); i++)
	{
		m_vecEqData[i]->fScale += 2.f * fTimeDelta;

		if (m_vecEqData[i]->fScale> 2.f)
		{
			m_vecEqData[i]->fAlpha -= 10.f * fTimeDelta;
			if (m_vecEqData[i]->fAlpha <= 0.f)
			{
				m_vecEqData[i]->fAlpha = 0.f;
			}
		}
	}

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 5.f * fTimeDelta);
	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CWhirlWind_EQ::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWhirlWind_EQ::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (_int i = 0; i < m_vecEqData.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;

		if(m_vecEqData[i]->bWhite == true)
			m_pTexture_White->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");
		else
			m_pTextureAlpha->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_vecEqData[i]->fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(0);
	}
		m_pSPHERECom->Render();
}

HRESULT CWhirlWind_EQ::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_E_Q_Slash"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Q_Black"), TEXT("Com_Texture_Black"), (CComponent**)&m_pTextureAlpha)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Q_White"), TEXT("Com_Texture_White"), (CComponent**)&m_pTexture_White)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Hit_Sphere"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind_EQ::SetUp_ConstantTable(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	_matrix		InstanceMatrix;
	if (m_vecEqData[iNumModel]->bTurn == true)
	{
		InstanceMatrix = XMMatrixIdentity() * XMMatrixScaling(m_vecEqData[iNumModel]->fScale, m_vecEqData[iNumModel]->fScale, m_vecEqData[iNumModel]->fScale) * XMMatrixRotationY(XMConvertToRadians(180.f));
	}

	else
	{
		InstanceMatrix = XMMatrixIdentity() * XMMatrixScaling(m_vecEqData[iNumModel]->fScale, m_vecEqData[iNumModel]->fScale, m_vecEqData[iNumModel]->fScale);
	}
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(InstanceMatrix *  m_pTransformCom->Get_WorldMatrix()));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWhirlWind_EQ * CWhirlWind_EQ::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CWhirlWind_EQ*		pInstance = new CWhirlWind_EQ(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CWhirlWind_EQ"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWhirlWind_EQ::Clone(void * pArg)
{
	CWhirlWind_EQ*		pInstance = new CWhirlWind_EQ(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CWhirlWind_EQ"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWhirlWind_EQ::Free()
{
	__super::Free();
}
