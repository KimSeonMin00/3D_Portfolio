#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector vPos;
		memcpy(&vPos, pArg, sizeof(_vector));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{

}

void CMonster::Late_Tick(_float fTimeDelta)
{
	m_bSelected = false;
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Airborne(_float fTimeDelta)
{
	m_fAirborneTime += fTimeDelta;

	if(m_fAirborneTime <= 0.5f)
		m_pTransformCom->Go_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 4.f * fTimeDelta);
	else
	{
		m_pTransformCom->Go_Direction(XMVectorSet(0.f, -1.f, 0.f, 0.f), 4.f * fTimeDelta);

		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= 0.f)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			m_bAirborne = false;
		}
	}
	
}

void CMonster::Drop(_float fTimeDelta)
{
	m_fDropTime += fTimeDelta;

	if (m_fDropTime <= 1.f)
	{
		m_pTransformCom->Go_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f * fTimeDelta);
		m_fHeight = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	else
	{
		m_pTransformCom->Go_Direction(XMVectorSet(0.f, -1.f, 0.f, 0.f), 5.f * m_fHeight * fTimeDelta);

		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= 0.f)
		{
			m_fDropTime = 0.f;
			m_bDrop = false;
		}
	}
}

void CMonster::Chase_Player(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Player")) == 0)
	{
		Safe_Release(pGameInstance);
		return;
	}

	CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

	if (pPlayer_Transform == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_AddRef(pPlayer_Transform);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

	Safe_Release(pPlayer_Transform);

	m_vMovePos = vPlayerPos;

	m_fMoveDistTotal = XMVectorGetX(XMVector3Length(vPlayerPos - vPos));

	m_fMoveDist = 0.f;

	Safe_Release(pGameInstance);
}

void CMonster::None_Overlap(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (iLayerSize != 0)
	{
		for (_uint i = 0; i < iLayerSize; i++)
		{
			CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_Transform"), i);

			if (pTransform != nullptr || pTransform != m_pTransformCom)
			{
				Safe_AddRef(pTransform);

				_vector vTargetPos = pTransform->Get_State(CTransform::STATE_POSITION);

				if (XMVectorGetX(XMVector3Length(vPos - vTargetPos)) < 1.f)
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + ((vPos - vTargetPos) * fTimeDelta));
				}

				Safe_Release(pTransform);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
