#include "stdafx.h"
#include "..\Public\WhirlWind_EQ.h"

#include "Collider.h"

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

	return S_OK;
}

void CWhirlWind_EQ::Tick(_float fTimeDelta)
{
	m_fLiveTime += fTimeDelta;

	if (m_fLiveTime >= 0.5f)
		m_bDead = true;

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

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pSPHERECom->Render();
}

HRESULT CWhirlWind_EQ::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
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

HRESULT CWhirlWind_EQ::SetUp_ConstantTable()
{
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
