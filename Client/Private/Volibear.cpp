#include "stdafx.h"
#include "..\Public\Volibear.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "HierarchyNode.h"

CVolibear::CVolibear(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMonster(pDevice, pDevice_Context)
{
}

CVolibear::CVolibear(const CVolibear & rhs)
	:CMonster(rhs)
{
}

HRESULT CVolibear::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolibear::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pRHNode = m_pModelCom->Find_HierarcyNodes("R_Buffbone_Glb_Hand_Loc");
	if (m_pRHNode == nullptr)
		return E_FAIL;

	m_pLHNode = m_pModelCom->Find_HierarcyNodes("L_Buffbone_Glb_Hand_Loc");
	if (m_pLHNode == nullptr)
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	m_pModelCom->SetUp_AnimationIndex(14);
	m_eState = STATE_IDLE;
	m_ePreState = STATE_IDLE;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CVolibear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bPattern1 == false)
	{
		m_fDelayTime += fTimeDelta;
		if (m_fDelayTime >= 3.f)
			m_bPattern1 = true;
	}

	//Key_Input(fTimeDelta);

	Check_Loop(fTimeDelta);

	if (m_bPattern1 == true)
	{
		if (m_bAirborne == false && m_pModelCom->Get_IsChange() == false)
			Pattern_4(fTimeDelta);
	}
	

	Change_State(fTimeDelta);

	Update_State(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Update_HandCollider();
	m_pSPHEREAttackRange->Update(m_pTransformCom->Get_WorldMatrix());
}

void CVolibear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_HitSphere")))->Collision_Sphere(m_pSPHEREAttackRange))
	{
		if (m_eState == STATE_ATTACK)
		{
			if (false == ((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pOBBRightHand))
				((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pOBBLeftHand);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CVolibear::Render()
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

		if (m_pAABBCom->Get_IsCollision() == true)
			m_pShaderCom->Begin(1);

		else
			m_pShaderCom->Begin(0);

		if(i != 7)//포로 메쉬 제외
			m_pModelCom->Render(i);
	}

	m_pAABBCom->Render();
	m_pSphereCom->Render();
	m_pOBBRightHand->Render();
	m_pOBBLeftHand->Render();
	m_pSPHEREAttackRange->Render();

	return S_OK;
}

HRESULT CVolibear::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Volibear"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.5f, 3.f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 1.5f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_HitBox"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);


	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_HitSphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_AttackRange"), (CComponent**)&m_pSPHEREAttackRange, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_RightHand"), (CComponent**)&m_pOBBRightHand, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_LeftHand"), (CComponent**)&m_pOBBLeftHand, &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVolibear::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CVolibear::Update_HandCollider()
{
	_float4x4  SocketMatrix;

	XMStoreFloat4x4(&SocketMatrix, m_pRHNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	_vector a, b;

	a = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1]));
	b = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2]));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], b);
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], a);

	XMStoreFloat4x4(&SocketMatrix, XMLoadFloat4x4(&SocketMatrix) * m_pTransformCom->Get_WorldMatrix());

	//m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pOBBRightHand->Update(XMLoadFloat4x4(&SocketMatrix));

	XMStoreFloat4x4(&SocketMatrix, m_pLHNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	a = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1]));
	b = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2]));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], b);
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], a);

	XMStoreFloat4x4(&SocketMatrix, XMLoadFloat4x4(&SocketMatrix) * m_pTransformCom->Get_WorldMatrix());

	//m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pOBBLeftHand->Update(XMLoadFloat4x4(&SocketMatrix));
}

void CVolibear::Key_Input(_float fTimeDelta)
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
		XMStoreFloat3(&vMoveDist, XMVector3Length(m_vMoveDir));
		m_fMoveDistTotal = vMoveDist.x;
		m_vMoveDir = XMVector3Normalize(m_vMoveDir);
		m_fMoveDist = 0.f;
	}

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_LBUTTON) & 0x80)
	{
		m_eState = STATE_ATTACK;

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

	if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	{
		m_bQState = true;
		m_bQAttack = false;
		m_fQTime = 0.f;
	}

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_eState = STATE_W;
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
		m_bIsChanneling = true;
	}

	if (m_bRState == false)
	{
		if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			m_eState = STATE_R;
			m_bRState = true;
			m_bIsChanneling = true;
			m_fRTime = 0.f;

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
			XMStoreFloat3(&vMoveDist, XMVector3Length(m_vMoveDir));
			m_fMoveDistTotal = vMoveDist.x;
			m_vMoveDir = XMVector3Normalize(m_vMoveDir);
			m_fMoveDist = 0.f;

			m_vOriginScale = m_pTransformCom->Get_Scaled();
			m_pTransformCom->Set_Scaled(XMLoadFloat3(&m_vOriginScale) * 1.5);
		}
	}

	Safe_Release(pGameInstance);
}

void CVolibear::Change_State(_float fTimeDelta)
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
			m_pTransformCom->LookAt(m_vMovePos);
			break;

		case STATE_ATTACK:
			if (m_iAttackIndex == 4)
				m_iAttackIndex = 3;
			else
				m_iAttackIndex++;
			break;
		}
	}
	m_ePreState = m_eState;
}

void CVolibear::Check_Loop(_float fTimeDelta)
{
	switch (m_eState)
	{
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
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_W:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		break;

	case STATE_W_BITE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		break;

	case STATE_E:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(1.5 * fTimeDelta);
		break;

	case STATE_R:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	default:
		break;
	}
}

void CVolibear::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		if (m_bStateChange == true)
		{
			if (m_eDoingState == STATE_MOVE)
			{
				if (m_bQState == true)
					if (m_bRState == true)
						m_iCurrentIndex = 41;
					else
						m_iCurrentIndex = 60;
				else
					if (m_bRState == true)
						m_iCurrentIndex = 33;
					else
						m_iCurrentIndex = 15;

				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else if (m_eDoingState == STATE_Q)
			{
				m_iCurrentIndex = 56;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else if (m_eDoingState == STATE_W)
			{
				m_iCurrentIndex = 69;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else if (m_eDoingState == STATE_W_BITE)
			{
				m_iCurrentIndex = 71;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
			else if (m_eDoingState == STATE_E)
			{
				m_iCurrentIndex = 73;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else if (m_eDoingState == STATE_R)
			{
				m_iCurrentIndex = 77;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bStateChange = false;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		else
		{		
			if (m_bQState == true)
			{
				if (m_bQState_Pre == true)
				{
					if (m_bIsState_In == true)
					{
						if (m_pModelCom->Get_Finished() == true)
						{
							if (m_bRState == true)
								m_iCurrentIndex = 41;
							else
								m_iCurrentIndex = 27;
							m_bIsState_In = false;
							m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
							m_pModelCom->Set_Initialize();
						}
					}

					if (m_bRState == true)
					{
						if (m_iCurrentIndex == 81 && m_pModelCom->Get_Finished() == true)
						{
							m_iCurrentIndex = 41;
							m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
							m_pModelCom->Set_Initialize();
						}
					}

					else
					{
						if (m_iCurrentIndex == 28 && m_pModelCom->Get_Finished() == true)
						{
							m_iCurrentIndex = 27;
							m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
							m_pModelCom->Set_Initialize();
						}
					}
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				}

				else if (m_bQState_Pre == false)
				{				
					if (m_bRState == true)
						m_iCurrentIndex = 81;
					else
						m_iCurrentIndex = 28;
					m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
					if (m_pModelCom->Get_IsChange() == false)
					{
						m_bQState_Pre = m_bQState;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
				}
			}

			else
			{
				if (m_bQState_Pre == true)
				{
					if (m_bRState == true)
						m_iCurrentIndex = 82;
					else
						m_iCurrentIndex = 64;
					m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
					if (m_pModelCom->Get_IsChange() == false)
					{
						m_bQState_Pre = m_bQState;
						m_bIsState_In = true;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
				}

				else
				{
					if (m_bIsState_In == true)
					{
						if (m_pModelCom->Get_Finished() == true)
						{			
							if (m_bRState == true)
								m_iCurrentIndex = 33;

							else
								m_iCurrentIndex = 14; 

							m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
							if (m_pModelCom->Get_IsChange() == false)
							{
								m_bIsState_In = false;
								m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
								m_pModelCom->Set_Initialize();
							}
							
						}
					}
				}
			}

			if (m_pModelCom->Get_IsChange() == false)
				m_pModelCom->Play_Animation(fTimeDelta);
		}
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

	case STATE_W_BITE:
		W_Bite_Skill(fTimeDelta);
		break;

	case STATE_E:
		E_Skill(fTimeDelta);
		break;

	case STATE_R:
		R_Skill(fTimeDelta);
		break;

	default:
		break;
	}
}

void CVolibear::Move(_float fTimeDelta)
{	
	if (m_bAirborne == false)
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));

	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_IDLE)
		{
			if (m_bQState == true)
				if (m_bRState == true)
					m_iCurrentIndex = 85;
				else
					m_iCurrentIndex = 61;
			else
				if (m_bRState == true)
					m_iCurrentIndex = 34;
				else
					m_iCurrentIndex = 23;

			m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
			if (m_pModelCom->Get_IsChange() == false)
			{
				m_bStateChange = false;
				m_bIsState_In = true;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			};
		}

		else if (m_eDoingState == STATE_ATTACK)
		{
			if (m_iAttackIndex == 3)
				m_iCurrentIndex = 37;

			else if (m_iAttackIndex == 4)
				m_iCurrentIndex = 38;

			m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
			if (m_pModelCom->Get_IsChange() == false)
			{
				m_bStateChange = false;
				m_bIsState_In = true;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}
		}

		else if (m_eDoingState == STATE_Q)
		{
			if (m_bRState == true)
				m_iCurrentIndex = 80;

			else
				m_iCurrentIndex = 59;
			m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 1.0);
			if (m_pModelCom->Get_IsChange() == false)
			{
				m_bStateChange = false;
				m_bIsState_In = true;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}
		}	
	}

	else
	{
		if (m_bQState == true)
		{
			if (m_bQState_Pre == true)
			{
				if (m_bRState == true)
				{
					if (m_iCurrentIndex == 85 && m_pModelCom->Get_Finished() == true)
					{
						m_iCurrentIndex = 39;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
				}

				else
				{
					if ((m_iCurrentIndex == 63 || m_iCurrentIndex == 61) && m_pModelCom->Get_Finished() == true)
					{
						m_iCurrentIndex = 65;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
				}

				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			}

			else if (m_bQState_Pre == false)
			{
				if (m_bRState == true)
					m_iCurrentIndex = 85;
				else
					m_iCurrentIndex = 63;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bQState_Pre = m_bQState;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}
		}

		else
		{
			if (m_bQState_Pre == true)
			{
				if (m_bRState == true)
					m_iCurrentIndex = 40;

				else
					m_iCurrentIndex = 66;
				m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
				if (m_pModelCom->Get_IsChange() == false)
				{
					m_bQState_Pre = m_bQState;
					m_bIsState_In = true;
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
					m_pModelCom->Set_Initialize();
				}
			}

			else
			{
				if (m_bIsState_In == true)
				{
					if (m_pModelCom->Get_Finished() == true)
					{
						if (m_bRState == true)
							m_iCurrentIndex = 34;

						else
							m_iCurrentIndex = 23;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
				}
			}
		}

		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::Attack(_float fTimeDelta)
{	
	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_Q)
		{
			m_iCurrentIndex = 58;
		}
		else
		{
			m_iCurrentIndex = m_iAttackIndex;
		}
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
			m_bQAttack = false;
			if (m_iAttackIndex == 4)
				m_iAttackIndex = 3;
			else
				m_iAttackIndex++;

			m_pModelCom->SetUp_AnimationIndex(m_iAttackIndex);
			m_pModelCom->Set_Initialize();
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::Q_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 26;
		m_pModelCom->Change_Animation(fTimeDelta, m_iCurrentIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bQState = false;
			m_bQState_Pre = false;
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
		}
	}
	else
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::W_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{			
		m_iCurrentIndex = 68;

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

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::W_Bite_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 29;

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

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::E_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 31;

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

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::R_Skill(_float fTimeDelta)
{
	if (m_fMoveDistTotal > m_fMoveDist)
	{
		m_pTransformCom->Go_Straight(_double(m_fMoveDistTotal * fTimeDelta));
		m_fMoveDist += m_fMoveDistTotal * fTimeDelta;
	}

	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 32;

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
			if (m_iCurrentIndex == 32)
				m_iCurrentIndex = 79;
			else if (m_iCurrentIndex == 79)
			{
				m_bIsChanneling = false;
				m_eState = STATE_IDLE;
				return;
			}

			m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
			m_pModelCom->Set_Initialize();
			
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVolibear::Chase_Player(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return;

	Safe_AddRef(pGameInstance);

	CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));

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

	m_pTransformCom->LookAt(vPlayerPos);

	m_fMoveDistTotal = XMVectorGetX(XMVector3Length(vPlayerPos - vPos));

	m_fMoveDist = 0.f;

	Safe_Release(pGameInstance);

}

void CVolibear::Pattern_1(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return;

	Safe_AddRef(pGameInstance);

	CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));

	if (pPlayer_Transform == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

	m_vMovePos = vPlayerPos;

	m_pTransformCom->LookAt(vPlayerPos);

	_float fDist = XMVectorGetX(XMVector3Length(vPlayerPos - vPos));

	if (m_bIsChanneling == false)
	{
		m_bQState = true;
		m_ePreState = STATE_MOVE;

		if (fDist >= 3.f)
		{
			m_eState = STATE_MOVE;
			m_fMoveSpeed = 4.0f;
		}

		else
		{
			m_eState = STATE_Q;
			m_bIsChanneling = true;
		}
	}

	else
	{
		if (m_pModelCom->Get_Finished() == true)
		{
			m_bIsChanneling = false;
			m_bQState = false;
			m_bQState_Pre = false;
			m_fDelayTime = 0.f;
			m_bPattern1 = false;
			m_eState = STATE_IDLE;
			m_fMoveSpeed = 2.0f;
		}
	}

	Safe_Release(pGameInstance);
}

void CVolibear::Pattern_2(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_ATTACK;
		m_bIsChanneling = true;
		m_iPattern_AttackTime++;
	}

	else
	{
		if (m_pModelCom->Get_Finished() == true)
		{
			if (m_iPattern_AttackTime < 2)
			{
				m_eState = STATE_ATTACK;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 2)
			{
				m_eState = STATE_Q;
				m_bQState = false;
				m_bQState_Pre = false;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 3)
			{
				m_eState = STATE_IDLE;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 4)
			{
				m_eState = STATE_E;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 5)
			{
				m_bIsChanneling = false;
				m_bQState = false;
				m_bQState_Pre = false;
				m_fDelayTime = 0.f;
				m_bPattern1 = false;
				m_iPattern_AttackTime = 0;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CVolibear::Pattern_3(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_ATTACK;
		m_bIsChanneling = true;
		m_iPattern_AttackTime++;
	}

	else
	{
		if (m_pModelCom->Get_Finished() == true)
		{
			if (m_iPattern_AttackTime ==1)
			{
				m_eState = STATE_W;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 2)
			{
				m_eState = STATE_W_BITE;
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 3)
			{
				m_bIsChanneling = false;
				m_bQState = false;
				m_bQState_Pre = false;
				m_fDelayTime = 0.f;
				m_bPattern1 = false;
				m_iPattern_AttackTime = 0;
				m_eState = STATE_IDLE;
			}
		}
	}
}

void CVolibear::Pattern_4(_float fTimeDelta)
{
	if (m_bIsChanneling == false)
	{
		m_eState = STATE_R;
		m_bIsChanneling = true;
		Chase_Player(fTimeDelta);
		m_iPattern_AttackTime++;
	}

	else
	{
		if (m_pModelCom->Get_Finished() == true)
		{
			if (m_iPattern_AttackTime == 1)
			{
				m_eState = STATE_R;
				if(m_iCurrentIndex == 79)
					m_iPattern_AttackTime++;
			}

			if (m_iPattern_AttackTime == 2)
			{
				m_eState = STATE_W;
				Chase_Player(fTimeDelta);
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 3)
			{
				m_eState = STATE_Q;
				Chase_Player(fTimeDelta);
				m_iPattern_AttackTime++;
			}
			else if (m_iPattern_AttackTime == 4)
			{
				m_bIsChanneling = false;
				m_bQState = false;
				m_bQState_Pre = false;
				m_fDelayTime = 0.f;
				m_bPattern1 = false;
				m_iPattern_AttackTime = 0;
				m_eState = STATE_IDLE;
			}
		}
	}
}

CVolibear * CVolibear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVolibear*		pInstance = new CVolibear(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVolibear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVolibear::Clone(void * pArg)
{
	CVolibear*		pInstance = new CVolibear(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVolibear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVolibear::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
