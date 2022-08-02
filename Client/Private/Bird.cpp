#include "stdafx.h"
#include "..\Public\Bird.h"
#include "GameInstance.h"

CBird::CBird(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMapObject_Anim(pDevice, pDevice_Context)
{
}

CBird::CBird(const CBird & rhs)
	:CMapObject_Anim(rhs)
{
}

HRESULT CBird::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBird::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_MapObject_Bird"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 5.f, 1.f));

	m_pModelCom->SetUp_AnimationIndex(1);

	return S_OK;
}

void CBird::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBird::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBird::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBird * CBird::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CBird*		pInstance = new  CBird(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  CBird"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBird::Clone(void * pArg)
{
	CBird*		pInstance = new CBird(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CBird"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBird::Free()
{
	__super::Free();
}
