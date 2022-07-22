#include "stdafx.h"
#include "..\Public\WhirlWind_Normal.h"
#include "Collider.h"
#include "GameInstance.h"

CWhirlWind_Normal::CWhirlWind_Normal(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CWhirlWind(pDevice, pDevice_Context)
{
}

CWhirlWind_Normal::CWhirlWind_Normal(const CWhirlWind_Normal & rhs)
	:CWhirlWind(rhs)
{
}

HRESULT CWhirlWind_Normal::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind_Normal::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_matrix  WorldMat;

		memcpy(&WorldMat, pArg, sizeof(_matrix));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);
		m_vMoveDir = WorldMat.r[2];
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	SCALEALPHA* tScaleAlpha = new SCALEALPHA;
	tScaleAlpha->bTurn = m_bTurn;
	m_vScaleAlpha.push_back(tScaleAlpha);
	m_bTurn = !m_bTurn;
	m_fAddMatrixTime = 0.f;

	return S_OK;
}

void CWhirlWind_Normal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAddMatrixTime += fTimeDelta;
	if (m_fAddMatrixTime >= 0.2f)
	{
		SCALEALPHA* tScaleAlpha = new SCALEALPHA;
		tScaleAlpha->bTurn = m_bTurn;
		m_vScaleAlpha.push_back(tScaleAlpha);
		m_bTurn = !m_bTurn;
		m_fAddMatrixTime = 0.f;
	}

	for (_int i = 0; i < m_vScaleAlpha.size(); i++)
	{
		m_vScaleAlpha[i]->fScale += 1.f * fTimeDelta;

		if (m_vScaleAlpha[i]->bTurn == false)
			m_vScaleAlpha[i]->fPosY += 1.f * fTimeDelta;
		else
			m_vScaleAlpha[i]->fPosY += 4.f * fTimeDelta;

		if (m_vScaleAlpha[i]->fAlpha > 0.f)
		{
			if (m_vScaleAlpha[i]->fPosY > 1.f)
			{
				m_vScaleAlpha[i]->fAlpha -= 2.f * fTimeDelta;		
			}


			else
			{
				m_vScaleAlpha[i]->fAlpha -= 1.f * fTimeDelta;
			}

			if (m_vScaleAlpha[i]->fAlpha <= 0.f)
			{
				m_vScaleAlpha[i]->fAlpha = 0.f;
			}
		}
	}

	m_fAddMatrixTime_2 += fTimeDelta;
	if (m_fAddMatrixTime_2 >= 0.2f)
	{
		SCALEALPHA* tScaleAlpha = new SCALEALPHA;
		
		tScaleAlpha->fAlpha = 0.5f;
		if (m_iTornadoPos == 0)
		{
			tScaleAlpha->fPosY = 2.f;
			m_iTornadoPos++;
		}

		else if (m_iTornadoPos == 1)
		{
			tScaleAlpha->fPosY = 0.f;
			m_iTornadoPos++;
		}

		else if (m_iTornadoPos == 2)
		{
			tScaleAlpha->fPosY = 1.f;
			m_iTornadoPos = 0;
		}

		m_vScaleAlpha_2.push_back(tScaleAlpha);
		m_fAddMatrixTime_2 = 0.f;
	}

	for (_int i = 0; i < m_vScaleAlpha_2.size(); i++)
	{
		m_vScaleAlpha_2[i]->fScale += 1.f * fTimeDelta;

		if (m_vScaleAlpha_2[i]->fAlpha > 0.1f)
		{
			m_vScaleAlpha_2[i]->fAlpha -= 1.f * fTimeDelta;
			if (m_vScaleAlpha_2[i]->fAlpha <= 0.f)
			{
				m_vScaleAlpha_2[i]->fAlpha = 0.f;
			}
		}
	}

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 24.f * fTimeDelta);
	m_pTransformCom->Go_Direction(m_vMoveDir, 5.f * fTimeDelta);
	m_fMoveDist += 5.f * fTimeDelta;

	if (m_fMoveDist >= 10.f)
		m_bDead = true;

	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CWhirlWind_Normal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWhirlWind_Normal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (_int i = 0; i < m_vScaleAlpha.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;

		m_pTextureAlpha->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		_float2 MoveTex = _float2(-0.f, 0.f);

		m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_vScaleAlpha[i]->fAlpha, sizeof(_float));

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(0);
	}

	for (_int i = 0; i < m_vScaleAlpha_2.size(); i++)
	{
		if (FAILED(SetUp_ConstantTable_2(i)))
			return E_FAIL;

		m_pModelCom_2->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", 0, aiTextureType_DIFFUSE);

		m_pTextureAlpha_2->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		_float2 MoveTex= _float2(-0.5f, 0.f);

		m_pShaderCom->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_vScaleAlpha_2[i]->fAlpha, sizeof(_float));

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 100.f/255.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Begin(2);

		m_pModelCom_2->Render(0);
	}

	//Render_Crack();

	m_pSPHERECom->Render();

	return S_OK;
}

HRESULT CWhirlWind_Normal::Render_Crack()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	_matrix		WorldMat;
	WorldMat = XMMatrixIdentity() * XMMatrixScaling(m_fCrack_Scale, m_fCrack_Scale, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	WorldMat.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.2f, 0.f, 0.f);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(WorldMat));
	m_pShaderCom_Rect->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pShaderCom_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTexture_Crack->Bind_OnShader(m_pShaderCom_Rect, "g_DiffuseTexture");

	m_pShaderCom_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom_Rect->Set_RawValue("g_Alpha", &m_fCrack_Alpha, sizeof(_float));

	m_pShaderCom_Rect->Begin(2);

	m_pCrack_Rect->Render();

	return S_OK;
}

HRESULT CWhirlWind_Normal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom_Rect)))
		return E_FAIL;


	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tornado"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pCrack_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Q_White"), TEXT("Com_Alpha_Texture"), (CComponent**)&m_pTextureAlpha)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tornado_2"), TEXT("Com_Model_2"), (CComponent**)&m_pModelCom_2)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tornado_Alpha"), TEXT("Com_Alpha_Texture_2"), (CComponent**)&m_pTextureAlpha_2)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Q_Crack"), TEXT("Com_Texture_Q_Crack"), (CComponent**)&m_pTexture_Crack)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Hit_Sphere"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind_Normal::SetUp_ConstantTable(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	_matrix		InstanceMatrix;
	if (m_vScaleAlpha[iNumModel]->bTurn == true)
	{
		InstanceMatrix = XMMatrixIdentity() * XMMatrixScaling(m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, m_vScaleAlpha[iNumModel]->fPosY, 0.f);
	}
	else
	{
		InstanceMatrix = XMMatrixIdentity() * XMMatrixScaling(m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale) * XMMatrixTranslation(0.f, m_vScaleAlpha[iNumModel]->fPosY, 0.f);
	}
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(InstanceMatrix *  m_pTransformCom->Get_WorldMatrix()));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CWhirlWind_Normal::SetUp_ConstantTable_2(_uint iNumModel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	_matrix		InstanceMatrix;

	InstanceMatrix = XMMatrixIdentity() * XMMatrixScaling(m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale, m_vScaleAlpha[iNumModel]->fScale) * XMMatrixTranslation(0.f, m_vScaleAlpha[iNumModel]->fPosY, 0.f);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(InstanceMatrix *  m_pTransformCom->Get_WorldMatrix()));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWhirlWind_Normal * CWhirlWind_Normal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CWhirlWind_Normal*		pInstance = new CWhirlWind_Normal(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CWhirlWind_Normal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWhirlWind_Normal::Clone(void * pArg)
{
	CWhirlWind_Normal*		pInstance = new CWhirlWind_Normal(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CWhirlWind_Normal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWhirlWind_Normal::Free()
{
	__super::Free();

	for (auto& ScaleAlpha : m_vScaleAlpha)
	{
		Safe_Delete(ScaleAlpha);
	}

	m_vScaleAlpha.clear();

	for (auto& ScaleAlpha : m_vScaleAlpha_2)
	{
		Safe_Delete(ScaleAlpha);
	}

	m_vScaleAlpha_2.clear();

	Safe_Release(m_pTexture_Crack);
	Safe_Release(m_pCrack_Rect);
	Safe_Release(m_pShaderCom_Rect);

	Safe_Release(m_pTextureAlpha_2);
	Safe_Release(m_pModelCom_2);

}

