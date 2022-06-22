#include "stdafx.h"
#include "..\Public\WhirlWind_Normal.h"
#include "Collider.h"

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


	_matrix  WorldMat;

	memcpy(&WorldMat, pArg, sizeof(_matrix));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);
	m_vMoveDir = WorldMat.r[2];

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CWhirlWind_Normal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Direction(m_vMoveDir, 15.f * fTimeDelta);
	m_fMoveDist += 15.f * fTimeDelta;

	if (m_fMoveDist >= 15.f)
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

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pSPHERECom->Render();
}

HRESULT CWhirlWind_Normal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
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

HRESULT CWhirlWind_Normal::SetUp_ConstantTable()
{
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
