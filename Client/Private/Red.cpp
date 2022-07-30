#include "stdafx.h"
#include "..\Public\Red.h"
#include "GameInstance.h"

CRed::CRed(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMonster(pDevice, pDevice_Context)
{
}

CRed::CRed(const CRed & rhs)
	:CMonster(rhs)
{
}

HRESULT CRed::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRed::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_iMonsterIndex = 2;

	//aggro : 15 non_aggro : 8 run : 3 attack : 4,5  death : 6 13 - a->n  17 n->a 19->spawn
	m_pModelCom->SetUp_AnimationIndex(10);
	m_eState = STATE_IDLE;
	m_ePreState = STATE_IDLE;

	m_fHealthPoint = m_fMaxHealth = 500.f;

	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));

	return S_OK;
}

void CRed::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_bAirborne == true)
	{
		Airborne(fTimeDelta);
		return;
	}

	if (m_bDrop == true)
	{
		Drop(fTimeDelta);
		return;
	}

	if (m_bStop == false && m_bStun == false)
		Check_Loop(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Player")) == 0)
	{
		m_eState = STATE_IDLE;
	}

	else
	{
		__super::Chase_Player(fTimeDelta);

		if (m_eState != STATE_DEATH)
		{
			if (m_fHealthPoint < m_fMaxHealth)
			{
				if (m_fMoveDistTotal < 2.f)
					m_eState = STATE_ATTACK;
				else
					m_eState = STATE_MOVE;
			}
			else
			{
				if (m_fMoveDistTotal < 5.f)
					m_eState = STATE_AGGRO;
				else
					m_eState = STATE_IDLE;
			}
		}

		if (m_fHealthPoint <= 0.f)
		{
			m_eState = STATE_DEATH;
		}
	}

	Safe_Release(pGameInstance);

	Change_State(fTimeDelta);

	if (m_bStop == false)
		Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CRed::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRed::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_BoneMatrices_OnShader(m_pShaderCom, "g_Bones", i);

		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Set_RawValue("g_vHitColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		if (m_bSelected == true)
			m_pShaderCom->Begin(1);

		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	m_pAABBCom->Render();
	m_pSphereCom->Render();

	return S_OK;
}

HRESULT CRed::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Red"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_HitBox"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);


	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_HitSphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
}

HRESULT CRed::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CRed::Change_State(_float fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		m_bStateChange = true;
		m_eDoingState = m_ePreState;

		switch (m_eState)
		{
		case STATE_IDLE:
			break;

		case STATE_MOVE:
			break;

		case STATE_ATTACK:
			break;
		}
	}
	m_ePreState = m_eState;
}

void CRed::Check_Loop(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_AGGRO:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_MOVE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_ATTACK:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_DEATH:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	default:
		break;
	}
}

void CRed::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		Idle(fTimeDelta);
		break;

	case STATE_AGGRO:
		Idle_Aggro(fTimeDelta);
		break;

	case STATE_MOVE:
		Move(fTimeDelta);
		break;

	case STATE_ATTACK:
		Attack(fTimeDelta);
		break;

	case STATE_DEATH:
		Death(fTimeDelta);
		break;

	default:
		break;
	}
}

void CRed::Idle(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_AGGRO)
			m_iCurrentIndex = 12;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_bState_In = true;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_bState_In == true)
		{
			if (m_pModelCom->Get_Finished())
			{
				m_iCurrentIndex = 10;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bState_In = false;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CRed::Idle_Aggro(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_vMovePos);

	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_IDLE)
			m_iCurrentIndex = 13;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_bState_In = true;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_bState_In == true)
		{
			if (m_pModelCom->Get_Finished())
			{
				m_iCurrentIndex = 9;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bState_In = false;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CRed::Move(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_vMovePos);
	__super::None_Overlap(fTimeDelta);
	m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));

	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 7;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CRed::Attack(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bHitPlayer = false;
		m_fHitTime = 0.f;

		m_iCurrentIndex = m_iAttackIndex;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_pModelCom->Get_Finished())
		{
			if (m_iAttackIndex == 2)
				m_iAttackIndex = 0;
			else
				m_iAttackIndex++;

			m_bHitPlayer =false;
			m_fHitTime = 0.f;
			m_pModelCom->SetUp_AnimationIndex(m_iAttackIndex);
			m_pModelCom->Set_Initialize();
		}

		if (m_bHitPlayer == false)
		{
			m_fHitTime += fTimeDelta;
			if (m_fHitTime >= 0.5f)
			{
				m_bHitPlayer = true;

				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Hit_Effect_Normal"), &m_vMovePos);

				RELEASE_INSTANCE(CGameInstance);
			}
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CRed::Death(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 3;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_pModelCom->Get_Finished())
		{
			m_bDead = true;
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

CRed * CRed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CRed*		pInstance = new  CRed(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  CRed"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRed::Clone(void * pArg)
{
	CRed*		pInstance = new CRed(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CRed"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRed::Free()
{
	__super::Free();
}
