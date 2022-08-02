#include "stdafx.h"
#include "..\Public\Duck.h"
#include "GameInstance.h"

CDuck::CDuck(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMapObject_Anim(pDevice, pDevice_Context)
{
}

CDuck::CDuck(const CDuck & rhs)
	:CMapObject_Anim(rhs)
{
}

HRESULT CDuck::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDuck::NativeConstruct(void * pArg)
{
	if(FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_MapObject_Duck"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 3.f, 1.f));
	m_pTransformCom->Set_Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	m_pModelCom->SetUp_AnimationIndex(1);

	return S_OK;
}

void CDuck::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDuck::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDuck::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CDuck * CDuck::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CDuck*		pInstance = new  CDuck(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  CDuck"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDuck::Clone(void * pArg)
{
	CDuck*		pInstance = new CDuck(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CDuck"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDuck::Free()
{
	__super::Free();
}
