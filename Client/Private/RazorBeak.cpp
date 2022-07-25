#include "stdafx.h"
#include "..\Public\RazorBeak.h"
#include "GameInstance.h"

CRazorBeak::CRazorBeak(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMonster(pDevice, pDevice_Context)
{
}

CRazorBeak::CRazorBeak(const CRazorBeak & rhs)
	:CMonster(rhs)
{
}

HRESULT CRazorBeak::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRazorBeak::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_iMonsterIndex = 1;

	m_pModelCom->SetUp_AnimationIndex(14);
	m_eState = STATE_IDLE;
	m_ePreState = STATE_IDLE;

	m_fHealthPoint = m_fMaxHealth = 300.f;

	m_pTransformCom->Set_Scaled(XMVectorSet(0.75f, 0.75f, 0.75f, 0.f));

	return S_OK;
}

void CRazorBeak::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bAirborne == true)
	{
		Airborne(fTimeDelta);
		return;
	}

	if (m_bStop == false && m_bStun == false)
		Check_Loop(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Layer_Size(LEVEL_GAMEPLAY, TEXT("Layer_Player")) == 0)
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
				if (m_fMoveDistTotal < 1.f)
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

	Change_State(fTimeDelta);

	if (m_bStop == false)
		Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CRazorBeak::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRazorBeak::Render()
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

		if (m_pAABBCom->Get_IsCollision() == true || m_bSelected == true)
			m_pShaderCom->Begin(1);

		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	m_pAABBCom->Render();
	m_pSphereCom->Render();

	return S_OK;
}

HRESULT CRazorBeak::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RazorBeak"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_HitBox"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);


	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_HitSphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
}

HRESULT CRazorBeak::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CRazorBeak::Change_State(_float fTimeDelta)
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

void CRazorBeak::Check_Loop(_float fTimeDelta)
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

void CRazorBeak::Update_State(_float fTimeDelta)
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

void CRazorBeak::Idle(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_AGGRO)
			m_iCurrentIndex = 7;

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
				m_iCurrentIndex = 14;
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

void CRazorBeak::Idle_Aggro(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_vMovePos);

	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_IDLE)
			m_iCurrentIndex = 19;

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
				m_iCurrentIndex = 2;
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

void CRazorBeak::Move(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_vMovePos);
	__super::None_Overlap(fTimeDelta);
	m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));

	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 5;

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

void CRazorBeak::Attack(_float fTimeDelta)
{
	if (m_bStateChange == true)
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

	else
	{
		if (m_pModelCom->Get_Finished())
		{
			if (m_iAttackIndex == 0)
				m_iAttackIndex = 21;
			else if (m_iAttackIndex == 21)
				m_iAttackIndex = 0;

			m_pModelCom->SetUp_AnimationIndex(m_iAttackIndex);
			m_pModelCom->Set_Initialize();
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CRazorBeak::Death(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 1;

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

CRazorBeak * CRazorBeak::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CRazorBeak*		pInstance = new  CRazorBeak(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created :  CRed"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRazorBeak::Clone(void * pArg)
{
	CRazorBeak*		pInstance = new CRazorBeak(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CRed"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRazorBeak::Free()
{
	__super::Free();
}
