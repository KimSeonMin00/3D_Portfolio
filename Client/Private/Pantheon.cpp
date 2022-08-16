#include "stdafx.h"
#include "..\Public\Pantheon.h"
#include "GameInstance.h"
#include "Pantheon_Shield.h"
#include "Player.h"
#include "Boss_HP.h"

CPantheon::CPantheon(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMonster(pDevice, pDevice_Context)
{
}

CPantheon::CPantheon(const CPantheon & rhs)
	: CMonster(rhs)
{
}

HRESULT CPantheon::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	m_pModelCom->SetUp_AnimationIndex(12);
	m_eState = STATE_IDLE;
	m_ePreState = STATE_IDLE;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 15.f, 1.f));*/
	m_pTransformCom->LookAt(XMVectorSet(-1.f, 0.f, 0.f, 1.f));
	
	m_pTransformCom->Set_Scaled(XMVectorSet(0.75f, 0.75f, 0.75f, 0.f));

	m_fMoveSpeed = 4.f;

	return S_OK;
}

void CPantheon::Tick(_float fTimeDelta)
{
	if (m_pHP != nullptr)
	{
		((CBoss_HP*)m_pHP)->Set_Ratio(m_fHealthPoint / m_fMaxHealth);
	}

	if (m_bStop == false)
		Check_Loop(fTimeDelta);

	if (m_bInit == false)
	{
		m_fInitTime += fTimeDelta;

		if (m_bThrowSpear == false)
		{
			if (m_fInitTime >= 7.f)
			{
				Throw_Spear(fTimeDelta);
				m_bThrowSpear = true;
			}
		}

		if (m_fInitTime >= 8.f)
		{
			m_eState = STATE_INIT;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.f, 15.f, 1.f));
		}
	}
	else if(m_bInit_2 == true)
	{
		if (m_fHealthPoint <= 0.f)
		{
			m_eState = STATE_DEATH;
		}

		else
		{
			if (m_bAirborne == true)
			{
				m_fAirborneTime += fTimeDelta;
				m_eState = STATE_IDLE;

				if (m_fAirborneTime >= 3.f)
				{
					m_bAirborne = false;
					m_bIsChanneling = false;
					m_bPatternFinished = true;
				}
			}

			else
			{
				//Key_Input(fTimeDelta);
				if (m_fInitTime < 5.f)
				{
					m_fInitTime += fTimeDelta;
					Normal_Pattern(fTimeDelta);
				}

				else
				{
					if (m_bPatternFinished == true)
					{
						while (true)
						{
							m_iCurrentPattern = rand() % 4;
							if (m_iCurrentPattern != m_iPrePattern)
							{
								m_iPrePattern = m_iCurrentPattern;
								m_bPatternFinished = false;
								break;
							}
						}
					}

					switch (m_iCurrentPattern)
					{
					case 0:
						Pattern_1(fTimeDelta);
						break;

					case 1:
						Pattern_2(fTimeDelta);
						break;

					case 2:
						Pattern_3(fTimeDelta);
						break;

					case 3:
						Pattern_4(fTimeDelta);
						break;

					default:
						break;
					}
				}
			}
		}
	}
	Change_State(fTimeDelta);

	if (m_bStop == false)
		Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	_matrix WorldMat = m_pTransformCom->Get_WorldMatrix();
	WorldMat.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f;

	m_pQ_Hitbox->Update(WorldMat);
}

void CPantheon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPantheon::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	m_pQ_Hitbox->Render();

	return S_OK;
}

HRESULT CPantheon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.5f, 3.f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 1.5f, 0.f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_HitBox"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);


	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_HitSphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 6.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_HitBox_Q"), (CComponent**)&m_pQ_Hitbox, &ColliderDesc)))
		return E_FAIL;
}

HRESULT CPantheon::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CPantheon::Key_Input(_float fTimeDelta)
{
	if (m_bIsChanneling == true)
		return;

	if (m_pModelCom->Get_IsChange() == true)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_eState = STATE_IDLE;
	}

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON) & 0x80)
	{
		m_eState = STATE_MOVE;
	}

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_LBUTTON) & 0x80)
	{
		m_eState = STATE_ATTACK;
	}

	if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	{
		m_eState = STATE_Q;
	}

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_eState = STATE_W;
	}

	if (pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
	{
		m_eState = STATE_E;
	}

	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
	{
		m_eState = STATE_R;
	}

	Safe_Release(pGameInstance);
}

void CPantheon::Change_State(_float fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		m_bStateChange = true;
		m_eDoingState = m_ePreState;
	}
	m_ePreState = m_eState;
}

void CPantheon::Check_Loop(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_INIT:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);

	case STATE_IDLE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);

	case STATE_MOVE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_ATTACK:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_Q:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5f * fTimeDelta);
		break;

	case STATE_Q_CHARGE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.f * fTimeDelta);
		break;

	case STATE_W:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5f * fTimeDelta);
		break;

	case STATE_E:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_R:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_KNOCKBACK:
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

void CPantheon::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_INIT:
		Init(fTimeDelta);
		break;

	case STATE_IDLE:
		Idle(fTimeDelta);
		break;

	case STATE_MOVE:
		Move(fTimeDelta);
		break;

	case STATE_ATTACK:
		Attack(fTimeDelta);
		break;

	case STATE_Q:
		Q_Skill(fTimeDelta);
		break;

	case STATE_Q_CHARGE:
		Q_Skill_Charge(fTimeDelta);
		break;

	case STATE_W:
		W_Skill(fTimeDelta);
		break;

	case STATE_E:
		E_Skill(fTimeDelta);
		break;

	case STATE_R:
		R_Skill(fTimeDelta);
		break;

	case STATE_KNOCKBACK:
		Knock_Back(fTimeDelta);
		break;

	case STATE_DEATH:
		Death(fTimeDelta);
		break;

	default:
		break;
	}
}

void CPantheon::Init(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_iCurrentIndex = 77;

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
			m_bIsChanneling = false;
			m_bInit = true;
			m_eState = STATE_IDLE;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_W_Effect"), &vPos);

			pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
			pGameInstance->PlaySounds(TEXT("Pantheon_W.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

			RELEASE_INSTANCE(CGameInstance);

			return;
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Throw_Spear(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		pTransform = m_pTransformCom->Get_WorldMatrix();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.f, 15.f, 1.f) + XMVectorSet(6.f, 8.f, 0.f, 0.f));
	m_pTransformCom->LookAt(XMVectorSet(45.f, 0.f, 15.f, 1.f));

	pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Pantheon_Q_Spear"), &m_pTransformCom->Get_WorldMatrix());
	pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
	pGameInstance->PlaySounds(TEXT("Pantheon_Q_Long.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pTransform.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, pTransform.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pTransform.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTransform.r[3]);

	RELEASE_INSTANCE(CGameInstance);
}

void CPantheon::Idle(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		_double ChangeDelay = 0.0;
		switch (m_eDoingState)
		{
		case STATE_INIT:
			m_iCurrentIndex = 79;
			ChangeDelay = 3.0;
			break;

		case STATE_ATTACK:
			m_iCurrentIndex = 15;
			ChangeDelay = 3.0;
			break;

		case STATE_MOVE:
			m_iCurrentIndex = 14;
			ChangeDelay = 3.0;
			break;

		case STATE_Q:
			m_iCurrentIndex = 42;
			break;

		case STATE_Q_CHARGE:
			m_iCurrentIndex = 47;
			break;

		case STATE_W:
			m_iCurrentIndex = 58;
			break;

		case STATE_E:
			m_iCurrentIndex = 68;
			break;

		default:
			m_bStateChange = false;
			m_bIdle_In = true;
			return;
			break;
		}

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, ChangeDelay);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_bIdle_In = true;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_bIdle_In == true)
		{
			if (m_pModelCom->Get_Finished())
			{
				if (m_bInit_2 == false && m_bInit == true)
				{
					m_bInit_2 = true;

					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Boss_HP"));

					_uint iIndex = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_UI")) - 1;

					m_pHP = pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_UI"), iIndex);

					Safe_AddRef(m_pHP);

					RELEASE_INSTANCE(CGameInstance);
				}

				m_iCurrentIndex = 12;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bIdle_In =false;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Move(_float fTimeDelta)
{
	__super::Chase_Player(fTimeDelta);
	m_pTransformCom->LookAt(m_vMovePos);
	m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));

	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 35;

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

void CPantheon::Attack(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bAttackIndex_Change = false;
		m_bHitPlayer = false;
		m_fHitTime = 0.f;

		if (m_eDoingState == STATE_W)
		{
			m_iCurrentIndex = 57;
			m_bStateChange = false;
			m_bW_3Attack = true;

			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
		else
		{
			m_iCurrentIndex = m_iAttackIndex;

			m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
			if (m_pModelCom->Get_IsChange() == false)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
				pGameInstance->PlaySounds(TEXT("Pantheon_Attack.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

				RELEASE_INSTANCE(CGameInstance);

				m_bStateChange = false;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}
		}
	}

	else
	{
		if (m_bW_3Attack == true)
		{
			if (m_iCurrentIndex == 57)
			{
				if (m_pModelCom->Get_Finished())
				{
					m_iCurrentIndex = 51;

					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
					pGameInstance->PlaySounds(TEXT("Pantheon_Attack.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

					RELEASE_INSTANCE(CGameInstance);

					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else if (m_iCurrentIndex == 51)
			{
				if (m_pModelCom->Get_Finished())
				{
					m_bW_3Attack = false;
					m_iAttackIndex = 0;
					m_bHitPlayer = false;
					m_fHitTime = 0.f;
					m_iCurrentIndex = m_iAttackIndex;

					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
					pGameInstance->PlaySounds(TEXT("Pantheon_Attack.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

					RELEASE_INSTANCE(CGameInstance);

					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}

				m_fHitTime += fTimeDelta;
				if (m_fHitTime >= 0.5f)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

					pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Hit_Effect"), &m_vMovePos);

					RELEASE_INSTANCE(CGameInstance);

					m_fHitTime = 0.;
				}
			}
		}
		else
		{
			if (m_pModelCom->Get_KeyFrame() == 25)
			{
				if (m_bAttackIndex_Change == false)
				{
					if (m_iAttackIndex == 4)
						m_iAttackIndex = 0;

					else
						m_iAttackIndex += 2;

					m_bAttackIndex_Change = true;
					m_iCurrentIndex = m_iAttackIndex;
					m_bHitPlayer = false;
					m_fHitTime = 0.f;
					m_pTransformCom->LookAt(m_vMovePos);
					return;
				}

				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bAttackIndex_Change = false;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		if (m_bHitPlayer == false)
		{
			m_fHitTime += fTimeDelta;
			if (m_fHitTime >= 0.5f)
			{
				m_bHitPlayer = true;

				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Hit_Effect"), &m_vMovePos);
				pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
				pGameInstance->PlaySounds(TEXT("Pantheon_Attack.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

				RELEASE_INSTANCE(CGameInstance);
			}
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Q_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_bSkillFinished = false;
		m_iCurrentIndex = 39;

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
			m_eState = STATE_IDLE;
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Q_Effect"), &m_pTransformCom->Get_WorldMatrix());
			pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
			pGameInstance->PlaySounds(TEXT("Pantheon_Q_Short.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

			if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pQ_Hitbox))
			{
				((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

				__super::Chase_Player(fTimeDelta);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Q_Hit_Effect"), &m_vMovePos);
			}

			RELEASE_INSTANCE(CGameInstance);

			m_bSkillFinished = true;

			return;
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Q_Skill_Charge(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_bSkillFinished = false;
		m_iCurrentIndex = 49;

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
			if (m_iCurrentIndex == 49)
			{
				m_iCurrentIndex = 44;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}

			else if (m_iCurrentIndex == 44)
			{
				m_eState = STATE_IDLE;
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Pantheon_Q_Spear"), &m_pTransformCom->Get_WorldMatrix());

				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(30.f) / XMConvertToRadians(60.f));

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Pantheon_Q_Spear"), &m_pTransformCom->Get_WorldMatrix());

				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-60.f) / XMConvertToRadians(60.f));

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_Pantheon_Q_Spear"), &m_pTransformCom->Get_WorldMatrix());

				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(30.f) / XMConvertToRadians(60.f));

				pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
				pGameInstance->PlaySounds(TEXT("Pantheon_Q_Long.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

				RELEASE_INSTANCE(CGameInstance);
				m_fInitTime = 0.f;
				m_bSkillFinished = true;
				return;
			}		
		}

		if (m_iCurrentIndex == 49)
		{
			__super::Chase_Player(fTimeDelta);
			m_pTransformCom->LookAt(m_vMovePos);
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::W_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_bSkillFinished = false;
		m_iCurrentIndex = 54;

		__super::Chase_Player(fTimeDelta);
		m_pTransformCom->LookAt(m_vMovePos);
		m_fWDash = m_fMoveDistTotal;

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
		m_pTransformCom->Go_Straight(_double(m_fWDash * 2.f * fTimeDelta));

		if (m_pModelCom->Get_Finished())
		{
			m_eState = STATE_IDLE;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 2.f * XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_W_Effect"),&vPos);

			pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
			pGameInstance->PlaySounds(TEXT("Pantheon_W.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

			RELEASE_INSTANCE(CGameInstance);

			m_bSkillFinished = true;
			return;
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::E_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_bSkillFinished = false;
		__super::Chase_Player(fTimeDelta);
		m_pTransformCom->LookAt(m_vMovePos);
		m_iCurrentIndex = 59;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_E_Shield"), &m_pTransformCom->Get_WorldMatrix());

			_uint iIndex = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Effect")) - 1;

			m_pShield = pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Effect"), iIndex);

			Safe_AddRef(m_pShield);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_E_Slash"), &m_pTransformCom->Get_WorldMatrix());

			RELEASE_INSTANCE(CGameInstance);

			m_bStateChange = false;
			m_fE_CastingTime = 0.f;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		m_fE_CastingTime += fTimeDelta;

		if (m_pModelCom->Get_Finished())
		{
			if (m_iCurrentIndex == 26)
			{
				m_eState = STATE_IDLE;
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_E_Swipe"), &m_pTransformCom->Get_WorldMatrix());
				pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
				pGameInstance->PlaySounds(TEXT("Pantheon_E_Swipe.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

				if (m_fMoveDistTotal <= 6.f)
				{
					_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
					_vector vPlayer = XMVector3Normalize(m_vMoveDir);

					if (XMVectorGetX(XMVector3Dot(vLook, vPlayer)) >= cos(XMConvertToRadians(30.f)))
					{
						((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

						CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

						_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

						pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Q_Hit_Effect"), &vPlayerPos);
					}
				}

				RELEASE_INSTANCE(CGameInstance);

				m_bSkillFinished = true;
				return;
			}

			if (m_fE_CastingTime <= 3.f)
				m_iCurrentIndex = 69;
			else
			{
				Safe_Release(m_pShield);
				m_iCurrentIndex = 26;
			}

			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}

		if (m_iCurrentIndex == 69)
		{
			m_f_E_SlashTime += fTimeDelta;

			if (m_f_E_SlashTime >= 0.1f)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_E_Slash"), &m_pTransformCom->Get_WorldMatrix());
				pGameInstance->StopSound(CSound_Device::CHANNEL_MONSTER);
				pGameInstance->PlaySounds(TEXT("Pantheon_E_Slash.wav"), CSound_Device::CHANNEL_MONSTER, 1.f);

				if(m_fMoveDistTotal <= 6.f)
				{
					_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
					_vector vPlayer = XMVector3Normalize(m_vMoveDir);

					if (XMVectorGetX(XMVector3Dot(vLook, vPlayer)) >= cos(XMConvertToRadians(30.f)))
					{
						((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

						CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

						_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

						pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Hit_Effect_Normal"), &vPlayerPos);
					}
				}

				RELEASE_INSTANCE(CGameInstance);

				m_f_E_SlashTime = 0.f;
			}

			__super::Chase_Player(fTimeDelta);
			if (m_fMoveDistTotal >= 3.f)
			{
				m_pTransformCom->Go_Direction(m_vMoveDir, 4.f * fTimeDelta);
			}
			((CPantheon_Shield*)m_pShield)->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::R_Skill(_float fTimeDelta)
{
}

void CPantheon::Knock_Back(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_bSkillFinished = false;
		m_iCurrentIndex = 8;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_fKnockBackTime = 0.f;
			m_bKnockBack = false;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}

	else
	{
		if (m_pModelCom->Get_Finished())
		{
			m_eState = STATE_IDLE;
			m_bSkillFinished = true;

			return;
		}

		if (m_bKnockBack == false)
		{
			m_fKnockBackTime += fTimeDelta;
			if (m_fKnockBackTime >= 0.3f)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Knock_Back(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

				((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Hit_Effect"), &m_vMovePos);

				RELEASE_INSTANCE(CGameInstance);

				m_bKnockBack = true;
			}

		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Death(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
		m_iCurrentIndex = 11;

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
			m_bIsChanneling = false;
			m_eState = m_eDoingState;
			m_bDead = true;
			return;
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Pattern_1(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_W;
		m_bIsChanneling = true;
		__super::Chase_Player(fTimeDelta);
		m_pTransformCom->LookAt(m_vMovePos);
		m_iPattern_AttackTime = 0;
		m_iPattern_AttackTime++;
	}
	else
	{
		if (m_iPattern_AttackTime == 1)
		{
			if (m_bSkillFinished == true)
			{
				m_eState = STATE_ATTACK;
				m_iPattern_AttackTime++;
			}
		}

		else if (m_iPattern_AttackTime == 2)
		{
			if (m_bW_3Attack == false && m_bAttackIndex_Change == true)
			{
				m_eState = STATE_Q;
				m_bAttackIndex_Change = false;
				m_iPattern_AttackTime++;
			}
		}
		else if (m_iPattern_AttackTime == 3)
		{	
			if (m_bSkillFinished == true)
			{
				m_eState = STATE_ATTACK;
				m_iPattern_AttackTime++;
			}
		}
		else if (m_iPattern_AttackTime == 4)
		{
			if (m_bAttackIndex_Change == true)
			{
				m_eState = STATE_E;
				m_bAttackIndex_Change = false;
				m_iPattern_AttackTime++;
			}
		}
		else
		{
			if (m_iCurrentIndex == 26 && m_bSkillFinished == true)
			{
				m_bIsChanneling = false;
				m_iPattern_AttackTime = 0;
				m_fInitTime = 0.f;
				m_bPatternFinished = true;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CPantheon::Pattern_2(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_MOVE;
		__super::Chase_Player(fTimeDelta);
		m_pTransformCom->LookAt(m_vMovePos);
		m_iPattern_AttackTime = 0;
		if (m_fMoveDistTotal <= 3.f)
		{
			m_eState = STATE_Q;
			m_iPattern_AttackTime++;
			m_bIsChanneling = true;
		}
	}
	else
	{
		if (m_iPattern_AttackTime == 1)
		{
			if (m_bSkillFinished == true)
			{
				m_eState = STATE_KNOCKBACK;
				m_iPattern_AttackTime++;
			}
		}

		else if (m_iPattern_AttackTime == 2)
		{
			if (m_bSkillFinished == true)
			{
				m_bIsChanneling = false;
				m_iPattern_AttackTime = 0;
				m_fInitTime = 0.f;
				m_bPatternFinished = true;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CPantheon::Pattern_3(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_E;
		m_iPattern_AttackTime = 0;
		m_iPattern_AttackTime++;
		m_bIsChanneling = true;

	}
	else
	{
		if (m_iPattern_AttackTime == 1)
		{
			if (m_bSkillFinished == true)
			{			
				m_eState = STATE_W;
				m_iPattern_AttackTime++;
			}
		}

		else if (m_iPattern_AttackTime == 2)
		{
			if (m_bSkillFinished == true)
			{
				m_bIsChanneling = false;
				m_iPattern_AttackTime = 0;
				m_fInitTime = 0.f;
				m_bPatternFinished = true;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CPantheon::Pattern_4(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_Q_CHARGE;
		m_iPattern_AttackTime = 0;
		m_iPattern_AttackTime++;
		m_bIsChanneling = true;

	}
	else
	{
		if (m_iPattern_AttackTime == 1)
		{
			if (m_bSkillFinished == true)
			{
				m_bIsChanneling = false;
				m_iPattern_AttackTime = 0;
				m_fInitTime = 0.f;
				m_bPatternFinished = true;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CPantheon::Normal_Pattern(_float fTimeDelta)
{
	__super::Chase_Player(fTimeDelta);

	if (m_fMoveDistTotal <= 3.f)
		m_eState = STATE_ATTACK;
	else
	{	
		if (m_eState == STATE_ATTACK && m_bAttackIndex_Change == false)
			m_eState = STATE_ATTACK;

		else
			m_eState = STATE_MOVE;
	}

}

CPantheon * CPantheon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon*		pInstance = new CPantheon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon::Clone(void * pArg)
{
	CPantheon*		pInstance = new CPantheon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon::Free()
{
	__super::Free();

	Safe_Release(m_pHP);
	Safe_Release(m_pQ_Hitbox);
}
