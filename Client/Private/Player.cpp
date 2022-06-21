#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "Terrain.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_ePreState = STATE_IDLE;
	m_eState = STATE_IDLE;
	m_pModelCom->SetUp_AnimationIndex(40);
	m_fQDistance = 3.f;

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	Change_State(fTimeDelta);

	Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());

	_matrix OffsetMat = m_pTransformCom->Get_WorldMatrix();
	OffsetMat.r[3] += m_fQDistance / 2.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pOBBCom->Update(OffsetMat);
	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

	m_pAABBCom->Render();
	m_pOBBCom->Render();
	m_pSPHERECom->Render();

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yasuo"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AABB*/
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, m_fQDistance);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(m_fQDistance * 2.f, m_fQDistance * 2.f, m_fQDistance * 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));
	
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Change_AnimtionIndex(_uint iIndex)
{
	m_iAnimationIndex = iIndex;
	m_pModelCom->SetUp_AnimationIndex(m_iAnimationIndex);
	m_pModelCom->Set_Initialize();
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_bIsChanneling == true)
		return;

	if (m_pModelCom->Get_IsChange() == true)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON) & 0x80)
	{
		m_eState = STATE_MOVE;
		m_bMove_Stop = false;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };
		_float3 vMoveDist = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_vMovePos = XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f);
		m_pTransformCom->LookAt(m_vMovePos);

		m_vMoveDir = m_vMovePos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat3(&vMoveDist ,XMVector3Length(m_vMoveDir));
		m_fMoveDistTotal = vMoveDist.x;
		m_vMoveDir = XMVector3Normalize(m_vMoveDir);
		m_fMoveDist = 0.f;
	}

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_LBUTTON) & 0x80)
	{
		m_eState = STATE_ATTACK;
		m_bWeapon_Out = true;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));

		m_pModelCom->SetUp_AnimationIndex(m_iAnimationIndex);
		m_pModelCom->Set_Initialize();
	}

	if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	{
		m_eState = STATE_Q;
		m_bWeapon_Out = true;
		m_bIsChanneling = true;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));

		m_iQ_Time++;

		if (m_iQ_Time == 1)
			m_iQAnimation_Index = 27;

		else if (m_iQ_Time == 2)
			m_iQAnimation_Index = 28;

		else if (m_iQ_Time == 3)
			m_iQAnimation_Index = 29;

		if (m_iQ_Time == 3)
			m_iQ_Time = 0;

		m_pModelCom->SetUp_AnimationIndex(m_iQAnimation_Index);
		m_pModelCom->Set_Initialize();
	}

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_eState = STATE_W;
		m_bWeapon_Out = true;
		m_bIsChanneling = true;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));
	}

	if (pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
	{
		m_eState = STATE_E;
		m_bWeapon_Out = true;
		m_bIsChanneling = true;
		m_fDashDist = 0.f;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));

		m_iEAnimation_Index = 35;
		m_pModelCom->SetUp_AnimationIndex(m_iEAnimation_Index);
		m_pModelCom->Set_Initialize();
	}

	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
	{
		m_eState = STATE_R;
		m_bWeapon_Out = true;
		m_bIsChanneling = true;

		_float3 vPositionPicking = { 0.f, 0.f, 0.f };

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));

		m_pModelCom->SetUp_AnimationIndex(36);
		m_pModelCom->Set_Initialize();
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Change_State(_float fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		m_bStateChange = true;
		m_eDoingState = m_ePreState;

		switch (m_eState)
		{
		case STATE_IDLE:
			break;

		case STATE_ATTACK:
			if (m_iAnimationIndex == 3)
				m_iAnimationIndex = 0;
			else
				m_iAnimationIndex++;
			m_pModelCom->Set_Initialize();
			break;

		case STATE_MOVE:
			m_pTransformCom->LookAt(m_vMovePos);
			if (m_bWeapon_Out == true)
			{
				m_bIsMotionChange = true;
			}
			break;
		}
	}
	m_ePreState = m_eState;
}

void CPlayer::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);

		if (m_bStateChange == true)
		{
			if (m_bWeapon_Out == true)
			{
				m_pModelCom->Change_Animation(fTimeDelta, 13, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_pModelCom->SetUp_AnimationIndex(13);
					m_pModelCom->Set_Initialize();
				}
			}
			else
			{
				m_pModelCom->Change_Animation(fTimeDelta, 40, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_pModelCom->SetUp_AnimationIndex(40);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		else
		{
			if (m_bWeapon_Out == true)
			{
				if (m_bSeathing == false)
				{
					m_pModelCom->SetUp_AnimationIndex(13);
				}

				if (m_pModelCom->Get_Finished() == true)
				{
					if (m_bSeathing == false)
					{
						m_bSeathing = true;
						m_pModelCom->SetUp_AnimationIndex(42);
						m_pModelCom->Set_Initialize();
					}

					else
					{
						m_bSeathing = false;
						m_bWeapon_Out = false;
						m_pModelCom->SetUp_AnimationIndex(40);
						m_pModelCom->Set_Initialize();
					}
				}
			}

			else
			{
				m_pModelCom->SetUp_AnimationIndex(40);
			}

			m_pModelCom->Play_Animation(fTimeDelta);
		}
		break;

	case STATE_MOVE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		Move(fTimeDelta);
		break;

	case STATE_ATTACK:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		Attack(fTimeDelta);
		break;

	case STATE_Q:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		Q_Skill(fTimeDelta);
		break;

	case STATE_W:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		W_Skill(fTimeDelta);
		break;

	case STATE_E:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		E_Skill(fTimeDelta);
		break;

	case STATE_R:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		R_Skill(fTimeDelta);
		break;

	default:
		break;
	}
}

void CPlayer::Move(_float fTimeDelta)
{
	if (m_fMoveDistTotal > m_fMoveDist)
	{
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));
		m_fMoveDist += m_fMoveSpeed * fTimeDelta;

		if (m_bStateChange == true)
		{
			if (m_bWeapon_Out == true)
			{
				m_pModelCom->Change_Animation(fTimeDelta, 21, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_pModelCom->SetUp_AnimationIndex(21);
					m_pModelCom->Set_Initialize();
				}
			}
			else
			{
				m_pModelCom->Change_Animation(fTimeDelta, 16, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_pModelCom->SetUp_AnimationIndex(16);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		else
		{
			if (m_bWeapon_Out == true)
			{
				if (m_pModelCom->Get_Finished() == true)
				{
					if (m_bIsMotionChange == true)
					{
						m_pModelCom->Change_Animation(fTimeDelta, 24, 3.0);
						if (m_pModelCom->Get_IsChange() == false)
						{
							m_bIsMotionChange = false;
							m_pModelCom->SetUp_AnimationIndex(24);
							m_pModelCom->Set_Initialize();
						}
					}

					else if (m_bIsMotionChange == false)
					{
						m_pModelCom->Change_Animation(fTimeDelta, 16, 3.0);
						if (m_pModelCom->Get_IsChange() == false)
						{
							m_bWeapon_Out = false;
							m_pModelCom->SetUp_AnimationIndex(16);
							m_pModelCom->Set_Initialize();
						}
					}
				}
			}
			else
				m_pModelCom->SetUp_AnimationIndex(16);
	
			if(m_pModelCom->Get_IsChange() == false)
				m_pModelCom->Play_Animation(fTimeDelta);
		}
	}

	else
	{
		m_eState = STATE_IDLE;
	}
}

void CPlayer::Attack(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_pModelCom->Change_Animation(fTimeDelta, m_iAnimationIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iAnimationIndex);
			m_pModelCom->Set_Initialize();
		}

	}

	else
	{
		if (m_pModelCom->Get_Finished())
		{
			if (m_iAnimationIndex == 3)
				m_iAnimationIndex = 0;
			else
				m_iAnimationIndex++;

			m_pModelCom->SetUp_AnimationIndex(m_iAnimationIndex);
			m_pModelCom->Set_Initialize();
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPlayer::Q_Skill(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
	{
		m_bIsChanneling = false;
		m_eState = m_eDoingState;
		return;
	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::W_Skill(_float fTimeDelta)
{

	if (m_bStateChange == true)
	{
		m_pModelCom->Change_Animation(fTimeDelta, 32, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(32);
			m_pModelCom->Set_Initialize();
		}
		return;
	}
	if (m_pModelCom->Get_Finished())
	{
		m_bIsChanneling = false;
		m_eState = m_eDoingState;
		return;
	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::E_Skill(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
	{
		m_bIsChanneling = false;
		m_bE_Q_Used = false;
		m_eState = STATE_IDLE;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (m_bE_Q_Used == false)
	{
		if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
		{
			m_iQ_Time++;

			if (m_iQ_Time == 3)
				m_iQ_Time = 0;

			m_bE_Q_Used = true;
			m_pModelCom->SetUp_AnimationIndex(25);
			m_pModelCom->Set_Initialize();
		}
	}

	Safe_Release(pGameInstance);

	if (m_fDashDist <= 3.f)
	{
		m_pTransformCom->Go_Straight(_double(5.f * fTimeDelta));
		m_fDashDist += 5.f * fTimeDelta;
	}

	else if (m_bE_Q_Used == false)
	{
		m_eState = STATE_IDLE;
		m_bIsChanneling = false;
		m_bE_Q_Used = false;
	}

	if (m_pModelCom->Get_IsChange() == false)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::R_Skill(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
	{
		m_eState = STATE_IDLE;
		m_bIsChanneling = false;
	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
