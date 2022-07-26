#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::NativeConstruct(void * pArg)
{


	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_eSTATE = STATE_PLAYER;

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_WHEEL))
	{
		_long		MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
	}

	//카메라 플레이어 방향으로 고정
	if(m_eSTATE == STATE_PLAYER)
		Look_Player();

	if (m_eSTATE == STATE_BOSS)
		Look_Boss();

	if (FAILED(__super::Bind_TransformMatrices()))
		return;

	Safe_Release(pGameInstance);

}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Look_Player()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

	if (pPlayerTransform == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_AddRef(pPlayerTransform);
	_vector vCamPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vCamPosition += XMVectorSet(0.f, 6.f, -3.f, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
	m_pTransformCom->LookAt(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pPlayerTransform);

	Safe_Release(pGameInstance);
}

void CCamera_Free::Look_Boss()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CTransform* pBossTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_Transform"));

	if (pBossTransform == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_AddRef(pBossTransform);
	_vector vCamPosition = pBossTransform->Get_State(CTransform::STATE_POSITION);
	vCamPosition += XMVectorSet(0.f, 1.5f, -6.f, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
	m_pTransformCom->LookAt(pBossTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));

	Safe_Release(pBossTransform);

	Safe_Release(pGameInstance);
}

CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CCamera_Free*	pInstance = new CCamera_Free(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone(void * pArg)
{
	CCamera_Free*	pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}
