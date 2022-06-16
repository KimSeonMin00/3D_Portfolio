#include "stdafx.h"
#include "..\Public\Volibear.h"
#include "GameInstance.h"
#include "Terrain.h"

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

	m_pModelCom->SetUp_AnimationIndex(14);
	m_eState = STATE_IDLE;

	return S_OK;
}

void CVolibear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fQTime += fTimeDelta;
	if (m_fQTime >= 10.0f)
		m_bQState = false;

	Key_Input(fTimeDelta);

	Change_State(fTimeDelta);

	Update_State(fTimeDelta);
}

void CVolibear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

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

void CVolibear::Key_Input(_float fTimeDelta)
{
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

		m_pModelCom->SetUp_AnimationIndex(m_iAttackIndex);
		m_pModelCom->Set_Initialize();
	}

	if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	{
		m_bQState = true;
		m_fQTime = 0.f;
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

void CVolibear::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);

		if (m_bStateChange == true)
		{
			if (m_eDoingState == STATE_MOVE)
			{
				if (m_bQState == true)
					m_iCurrentIndex = 60;
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
							m_iCurrentIndex = 27;
							m_bIsState_In = false;
							m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
							m_pModelCom->Set_Initialize();
						}
					}

					if (m_iCurrentIndex == 28 && m_pModelCom->Get_Finished() == true)
					{
						m_iCurrentIndex = 27;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				}

				else if (m_bQState_Pre == false)
				{
					
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
							m_bIsState_In = false;
							m_pModelCom->SetUp_AnimationIndex(14);
							m_pModelCom->Set_Initialize();
						}
					}
				}
			}

			if (m_pModelCom->Get_IsChange() == false)
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
			m_pModelCom->Check_Looped(fTimeDelta);
		Attack(fTimeDelta);
		break;

	default:
		break;
	}
}

void CVolibear::Move(_float fTimeDelta)
{
	if (m_fMoveDistTotal > m_fMoveDist)
	{
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));
		m_fMoveDist += m_fMoveSpeed * fTimeDelta;

		if (m_bStateChange == true)
		{
			if (m_eDoingState == STATE_IDLE)
			{
				if (m_bQState == true)
					m_iCurrentIndex = 61;
				else
					m_iCurrentIndex = 23;

				m_bStateChange = false;
				m_bIsState_In = true;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}

			if (m_eDoingState == STATE_ATTACK)
			{
				if (m_iAttackIndex == 3)
					m_iCurrentIndex = 37;

				else if (m_iAttackIndex == 4)
					m_iCurrentIndex = 38;

				m_bStateChange = false;
				m_bIsState_In = true;
				m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				m_pModelCom->Set_Initialize();
			}
		}

		else
		{
			if (m_bQState == true)
			{
				if (m_bQState_Pre == true)
				{
					if ((m_iCurrentIndex == 63 || m_iCurrentIndex == 61) && m_pModelCom->Get_Finished() == true)
					{
						m_iCurrentIndex = 65;
						m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
						m_pModelCom->Set_Initialize();
					}
					m_pModelCom->SetUp_AnimationIndex(m_iCurrentIndex);
				}

				else if (m_bQState_Pre == false)
				{
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
							m_bIsState_In = false;
							m_pModelCom->SetUp_AnimationIndex(23);
							m_pModelCom->Set_Initialize();
						}
					}
				}
			}

			if (m_pModelCom->Get_IsChange() == false)
				m_pModelCom->Play_Animation(fTimeDelta);
		}
	}

	else
	{
		m_eState = STATE_IDLE;
	}
}

void CVolibear::Attack(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_pModelCom->Change_Animation(fTimeDelta, m_iAttackIndex, 3.0);
		if (m_pModelCom->Get_IsChange() == false)
		{
			m_bStateChange = false;
			m_pModelCom->SetUp_AnimationIndex(m_iAttackIndex);
			m_pModelCom->Set_Initialize();
		}

	}

	else
	{
		if (m_pModelCom->Get_Finished())
		{
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

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
