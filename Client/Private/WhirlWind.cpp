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

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
	if (iLayerSize != 0)
	{
		for (_uint i = 0; i < iLayerSize; i++)
		{
			if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_HitSphere"), i))->Collision_Sphere(m_pSPHERECom))
			{

				if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_HitBox"), i))->Collision_AABB(m_pSPHERECom))
				{
					CMonster* pMonster = (CMonster*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Monster"), i);
					if (pMonster == nullptr)
						return;

					_bool bHit = false;
					for (auto& pIndex : m_vecMonsterIndex)
					{
						if (pIndex == pMonster)
						{
							bHit = true;
							break;
						}
					}

					if (bHit == true)
						break;

					Safe_AddRef(pMonster);

					if (m_bAirborne == true)
					{
						pMonster->Set_Airborne();
					}

					if (pMonster->Damaged(50.f) == true)
					{
						CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_Transform"), i);

						pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_Q_Hit_Effect"), &pTransform->Get_State(CTransform::STATE_POSITION));
					}

					m_vecMonsterIndex.push_back(pMonster);
				}
			}		
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWhirlWind::Render()
{
	return S_OK;
}

void CWhirlWind::Clear_MonsterIndex()
{
	for (auto& pIndex : m_vecMonsterIndex)
	{
		Safe_Release(pIndex);
	}

	m_vecMonsterIndex.clear();
}

void CWhirlWind::Free()
{
	__super::Free();

	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureAlpha);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pRendererCom);
}
