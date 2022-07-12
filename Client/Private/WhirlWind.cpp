#include "stdafx.h"
#include "WhirlWind.h"

#include "GameInstance.h"
#include "Monster.h"

CWhirlWind::CWhirlWind(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CWhirlWind::CWhirlWind(const CWhirlWind & rhs)
	:CGameObject(rhs)
{
}

HRESULT CWhirlWind::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhirlWind::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWhirlWind::Tick(_float fTimeDelta)
{
	return;
}

void CWhirlWind::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_HitSphere")))->Collision_Sphere(m_pSPHERECom))
	//{
	//	
	//	if (((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_HitBox")))->Collision_AABB(m_pSPHERECom))
	//	{
	//		CMonster* pMonster = (CMonster*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	//		if (pMonster == nullptr)
	//			return;
	//		Safe_AddRef(pMonster);

	//		pMonster->Set_Airborne();

	//		Safe_Release(pMonster);
	//		
	//	}
	//	
	//}

	//RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWhirlWind::Render()
{
	return S_OK;
}

void CWhirlWind::Free()
{
	__super::Free();

	Safe_Release(m_pSPHERECom);

	if(m_pShaderCom != nullptr)
		Safe_Release(m_pShaderCom);

	if(m_pModelCom != nullptr)
		Safe_Release(m_pModelCom);

	Safe_Release(m_pRendererCom);
}
