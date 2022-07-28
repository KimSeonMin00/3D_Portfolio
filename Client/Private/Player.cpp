#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "Terrain.h"
#include "WhirlWind_Normal.h"
#include "HierarchyNode.h"
#include "Monster.h"

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

	_float3 pPos;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(12.f, 0.f, 2.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 5.f, 1.f));
	XMStoreFloat3(&pPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pSwordNode = m_pModelCom->Find_HierarcyNodes("Buffbone_Glb_Weapon_1");
	if (m_pSwordNode == nullptr)
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	m_ePreState = STATE_IDLE;
	m_eState = STATE_IDLE;
	m_pModelCom->SetUp_AnimationIndex(40);
	m_fQDistance = 3.f;

	m_pTransformCom->Set_Scaled(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_bGrab == false)
	{
		Key_Input(fTimeDelta);

		Change_State(fTimeDelta);

		Update_State(fTimeDelta);
	}

	if (m_bFall == true)
		Fall(fTimeDelta);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	Update_SwordCollider();
	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pHitSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	if (m_bHit == false)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
		if (iLayerSize != 0)
		{
			for (_uint i = 0; i < iLayerSize; i++)
			{
				if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_HitSphere"), i))->Collision_Sphere(m_pSPHERECom))
				{
					if (m_eState == STATE_Q || m_eState == STATE_ATTACK)
					{
						if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_HitBox"), i))->Collision_AABB(m_pOBBCom))
							Hit_Monster(i);
					}
				}
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}

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

		if (m_pAABBCom->Get_IsCollision() == true)
		{
			m_pShaderCom->Set_RawValue("g_vHitColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
			m_pShaderCom->Begin(1);
		}

		if (i != 0)//피리 메쉬 제외
			m_pModelCom->Render(i);
	}

	m_pAABBCom->Render();
	m_pOBBCom->Render();
	m_pSPHERECom->Render();
	m_pHitSphereCom->Render();

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Yasuo"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AABB*/
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_HitBox"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vScale = _float3(1.2f, 2.f, 1.2f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);


	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_HitSphere"), (CComponent**)&m_pHitSphereCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(3.f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Attack_HitBox"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(7.f, 7.f, 7.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Attack_Range"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
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

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

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

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

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

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));
		pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_Q_Effect"), &m_pTransformCom->Get_WorldMatrix());
		m_iQ_Time++;

		if (m_iQ_Time == 1)
			m_iQAnimation_Index = 27;

		else if (m_iQ_Time == 2)
		{
			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_Q_Passive"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_iQAnimation_Index = 28;
		}

		else if (m_iQ_Time == 3)
		{
			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_WhirlWind_Normal"), &m_pTransformCom->Get_WorldMatrix());
			m_iQAnimation_Index = 29;
		}

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

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

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

		CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

		if (nullptr != pTerrain)
		{
			Safe_AddRef(pTerrain);
			vPositionPicking = pTerrain->Get_PickingPosition();
			Safe_Release(pTerrain);
		}

		m_pTransformCom->LookAt(XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));
		pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_E_Effect"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_iEAnimation_Index = 35;
		m_pModelCom->SetUp_AnimationIndex(m_iEAnimation_Index);
		m_pModelCom->Set_Initialize();
	}

	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
	{
		Cast_R(fTimeDelta);
	}

	Safe_Release(pGameInstance);
}

_bool CPlayer::Cast_R(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vPositionPicking = { 0.f, 0.f, 0.f };

	CTerrain* pTerrain = (CTerrain*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_BackGround"), 0);

	if (nullptr != pTerrain)
	{
		Safe_AddRef(pTerrain);
		vPositionPicking = pTerrain->Get_PickingPosition();
		Safe_Release(pTerrain);
	}

	_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
	if (iLayerSize != 0)
	{
		for (_uint i = 0; i < iLayerSize; i++)
		{
			CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_Transform"), i);

			if (pTransform != nullptr)
			{
				Safe_AddRef(pTransform);

				_vector vTargetPos = pTransform->Get_State(CTransform::STATE_POSITION);
				_float vDist = XMVectorGetX(XMVector3Length(vTargetPos - XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f)));
				if (vDist < 5.f)
				{
					if (((CMonster*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Monster"), i))->Get_Airborne() == true)
					{
						Safe_AddRef(pTransform);
						m_MonsterPosList.push_back(pTransform);
					}
				}
				Safe_Release(pTransform);
			}
		}

		if (m_MonsterPosList.size() == 0)
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
	}

	m_eState = STATE_R;
	m_bWeapon_Out = true;
	m_bIsChanneling = true;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPositionPicking.x, vPositionPicking.y, vPositionPicking.z, 1.f));

	m_pModelCom->SetUp_AnimationIndex(36);
	m_pModelCom->Set_Initialize();

	RELEASE_INSTANCE(CGameInstance);

	return true;
}

void CPlayer::Change_State(_float fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		m_bStateChange = true;
		m_eDoingState = m_ePreState;
		Initialize_Hit();

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
			m_pModelCom->Check_Looped(3.f * fTimeDelta);
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

void CPlayer::Update_SwordCollider()
{
	_float4x4  SocketMatrix;

	XMStoreFloat4x4(&SocketMatrix, m_pSwordNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	_vector a = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1]));
	_vector b = XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2]));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], b);
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], a);
	XMStoreFloat3((_float3*)&SocketMatrix.m[3], XMLoadFloat3((_float3*)&SocketMatrix.m[3]) - XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])) * 1.f);

	XMStoreFloat4x4(&SocketMatrix, XMLoadFloat4x4(&SocketMatrix) * m_pTransformCom->Get_WorldMatrix());
	m_SwordWorldMat = XMLoadFloat4x4(&SocketMatrix);

	//m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pOBBCom->Update(XMLoadFloat4x4(&SocketMatrix));

	/*_matrix OffsetMat = m_pTransformCom->Get_WorldMatrix();
	OffsetMat.r[3] += m_fQDistance / 2.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pOBBCom->Update(OffsetMat);*/
}

void CPlayer::Move(_float fTimeDelta)
{
	if (m_fMoveDistTotal > m_fMoveDist)
	{
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta), m_pNavigationCom);
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
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
			if (iLayerSize != 0)
			{
				for (_uint i = 0; i < iLayerSize; i++)
				{
					CMonster* pMonster = ((CMonster*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Monster"), i));

					Safe_AddRef(pMonster);

					pMonster->Set_BeHit(false);

					Safe_Release(pMonster);
				}
			}

			RELEASE_INSTANCE(CGameInstance);

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
		Initialize_Hit();

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
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (m_bE_Q_Used == false)
	{
		m_fAfterImageTime += fTimeDelta;

		if (m_fAfterImageTime >= 0.1f)
		{
			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_E_AF"), &m_pTransformCom->Get_WorldMatrix());
			m_fAfterImageTime = 0.f;
		}

		if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
		{

			m_iQ_Time++;

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (m_iQ_Time == 3)
			{
				vPos = XMVectorSetW(vPos, 2.f);
				m_iQ_Time = 0;
			}

			Initialize_Hit();
			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Skill"), TEXT("Prototype_GameObject_WhirlWind_EQ"), &vPos);			
			m_bE_Q_Used = true;
			m_pModelCom->SetUp_AnimationIndex(25);
			m_pModelCom->Set_Initialize();
		}
	}
	else
	{
		if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			if (Cast_R(fTimeDelta) == true)
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}

	Safe_Release(pGameInstance);

	if (m_bE_Q_Used == true)
	{
		if (m_fDashDist <= 4.f)
		{
			m_pTransformCom->Go_Straight(_double(8.f * fTimeDelta));
			m_fDashDist += 8.f * fTimeDelta;
		}

		if (m_pModelCom->Get_Finished())
		{
			m_bIsChanneling = false;
			m_bE_Q_Used = false;
			m_eState = STATE_IDLE;
			m_fAfterImageTime = 0.1f;
		}

	}

	else
	{
		if (m_fDashDist <= 4.f)
		{
			m_pTransformCom->Go_Straight(_double(8.f * fTimeDelta));
			m_fDashDist += 8.f * fTimeDelta;
		}

		else
		{
			m_eState = STATE_IDLE;
			m_bIsChanneling = false;
			m_bE_Q_Used = false;
			m_fAfterImageTime = 0.1f;
		}
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

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		for (auto& iter = m_MonsterPosList.begin(); iter != m_MonsterPosList.end();)
		{
			_vector vPos = (*iter)->Get_State(CTransform::STATE_POSITION);
			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_R_Effect"), &vPos);
			
			Safe_Release(*iter);
			iter = m_MonsterPosList.erase(iter);
		}

		RELEASE_INSTANCE(CGameInstance);

	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Fall(_float fTimeDelta)
{
	m_pTransformCom->Go_Backward(4.f*fTimeDelta);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (XMVectorGetY(vPos) > 0.f)
	{
		vPos = vPos + XMVectorSet(0.f, m_fYdir * fTimeDelta, 0.f, 0.f);
		m_fYdir -= 10.f * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	else
	{
		vPos = XMVectorSetY(vPos, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_bFall = false;
		m_bGrab = false;
	}
}

void CPlayer::Hit_Monster(_uint iIndex)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CMonster* pMonster = ((CMonster*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Monster"), iIndex));

	Safe_AddRef(pMonster);

	if (pMonster->Get_BeHit() == false)
	{
		if (pMonster->Damaged(m_fDamage) == true)
		{
			CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), TEXT("Com_Transform"), iIndex);

			if(m_eState == STATE_ATTACK)
				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_Attack_Effect"), &pTransform->Get_State(CTransform::STATE_POSITION));

			else if(m_eState == STATE_Q)
				pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Yasuo_Q_Hit_Effect"), &pTransform->Get_State(CTransform::STATE_POSITION));
		}
	}

	Safe_Release(pMonster);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Initialize_Hit()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLayerSize = pGameInstance->Get_Layer_Size(m_iLevel, TEXT("Layer_Monster"));
	if (iLayerSize != 0)
	{
		for (_uint i = 0; i < iLayerSize; i++)
		{
			CMonster* pMonster = ((CMonster*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Monster"), i));

			Safe_AddRef(pMonster);

			pMonster->Set_BeHit(false);

			Safe_Release(pMonster);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
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

	for (auto& pMonsterPos : m_MonsterPosList)
		Safe_Release(pMonsterPos);

	m_MonsterPosList.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pHitSphereCom);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
