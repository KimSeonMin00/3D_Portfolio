#include "stdafx.h"
#include "..\Public\Pantheon.h"
#include "GameInstance.h"

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
	m_ePreState = STATE_INIT;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.f, 15.f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	m_pTransformCom->Set_Scaled(XMVectorSet(0.75f, 0.75f, 0.75f, 0.f));

	return S_OK;
}

void CPantheon::Tick(_float fTimeDelta)
{


	if (m_bStop == false)
		Check_Loop(fTimeDelta);

	if (m_fHealthPoint <= 0.f)
	{
		m_eState = STATE_DEATH;
	}

	else
	{
		//Key_Input(fTimeDelta);
		if (m_fInitTime < 5.f)
			m_fInitTime += fTimeDelta;

		else
		{
			if (m_pModelCom->Get_IsChange() == false)
			{
				Pattern_1(fTimeDelta);
			}
		}
	}
	Change_State(fTimeDelta);

	if (m_bStop == false)
		Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
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

	case STATE_W:
		W_Skill(fTimeDelta);
		break;

	case STATE_E:
		E_Skill(fTimeDelta);
		break;

	case STATE_R:
		R_Skill(fTimeDelta);
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
			m_eState = STATE_IDLE;
			return;
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
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
					m_iCurrentIndex = m_iAttackIndex;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
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

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::Q_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_bIsChanneling = true;
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
			m_bIsChanneling = false;
			m_eState = m_eDoingState;
			return;
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
		m_iCurrentIndex = 54;

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
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * 1.5f *fTimeDelta));

		if (m_pModelCom->Get_Finished())
		{
			m_bIsChanneling = false;
			m_eState = m_eDoingState;
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
		m_iCurrentIndex = 59;

		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
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
				m_bIsChanneling = false;
				m_eState = m_eDoingState;
				return;
			}

			if (m_fE_CastingTime <= 3.f)
				m_iCurrentIndex = 74;
			else
				m_iCurrentIndex = 26;

			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPantheon::R_Skill(_float fTimeDelta)
{
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
			if (m_pModelCom->Get_Finished())
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
				m_eState = STATE_ATTACK;
				m_iPattern_AttackTime++;			
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
			if (m_iCurrentIndex == 26 && m_pModelCom->Get_Finished())
			{
				m_bIsChanneling = false;
				m_iPattern_AttackTime = 0;
				m_fInitTime = 0.f;
				m_eState = STATE_IDLE;
			}
		}
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
}
