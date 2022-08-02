#include "stdafx.h"
#include "..\Public\Owl.h"
#include "GameInstance.h"

COwl::COwl(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMapObject_Anim(pDevice, pDevice_Context)
{
}

COwl::COwl(const COwl & rhs)
	:CMapObject_Anim(rhs)
{
}

HRESULT COwl::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT COwl::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_MapObject_Owl"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(11.f, 2.5f, 11.f, 1.f));
	m_pTransformCom->Set_Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	m_pModelCom->SetUp_AnimationIndex(1);

	return S_OK;
}

void COwl::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void COwl::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT COwl::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

COwl * COwl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	COwl*		pInstance = new  COwl(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  COwl"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COwl::Clone(void * pArg)
{
	COwl*		pInstance = new COwl(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : COwl"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COwl::Free()
{
	__super::Free();
}
