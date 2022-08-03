#include "stdafx.h"
#include "..\Public\Duckie.h"
#include "GameInstance.h"

CDuckie::CDuckie(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMapObject_Anim(pDevice, pDevice_Context)
{
}

CDuckie::CDuckie(const CDuckie & rhs)
	: CMapObject_Anim(rhs)
{
}

HRESULT CDuckie::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDuckie::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_MapObject_Duckie"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.5f, 0.f, 3.5f, 1.f));
	m_pTransformCom->Set_Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	m_iObjectIndex = 3;
	m_pModelCom->SetUp_AnimationIndex(1);

	return S_OK;
}

void CDuckie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDuckie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDuckie::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CDuckie * CDuckie::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CDuckie*		pInstance = new  CDuckie(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  CDuckie"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDuckie::Clone(void * pArg)
{
	CDuckie*		pInstance = new CDuckie(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CDuckie"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDuckie::Free()
{
	__super::Free();
}
