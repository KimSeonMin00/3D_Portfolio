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

	m_pTransformCom->Set_Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	for (_int i = 0; i < 10; i++)
	{
		m_vMatrix.push_back(XMMatrixIdentity()* XMMatrixTranslation(0.f, 0.1f * (_float)i, 0.f));
	}

	m_iNumModel = 10;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CWhirlWind_Normal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Set_Scaled(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
	m_fScale += fTimeDelta;
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 5.f * fTimeDelta);
	m_pTransformCom->Go_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	m_pTransformCom->Go_Direction(m_vMoveDir, 5.f * fTimeDelta);
	m_fMoveDist += 5.f * fTimeDelta;

	if (m_fMoveDist >= 5.f)
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

	for (_int i = 0; i < m_iNumModel; i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;
		
		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", 0, aiTextureType_DIFFUSE);

		m_pTextureAlpha->Bind_OnShader(m_pShaderCom, "g_AlphaTexture");

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(0);

	}

	m_pSPHERECom->Render();

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

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tornado"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tornado_Alpha"), TEXT("Com_Alpha_Texture"), (CComponent**)&m_pTextureAlpha)))
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
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_vMatrix[iNumModel]));

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
}
