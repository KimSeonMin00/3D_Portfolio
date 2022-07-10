#include "stdafx.h"
#include "..\Public\Voli_Ghost.h"
#include "GameInstance.h"

CVoli_Ghost::CVoli_Ghost(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CMonster(pDevice, pDevice_Context)
{
}

CVoli_Ghost::CVoli_Ghost(const CVoli_Ghost & rhs)
	:CMonster(rhs)
{
}

HRESULT CVoli_Ghost::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_Ghost::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	m_pModelCom->SetUp_AnimationIndex(14);
	m_eState = STATE_MOVE;
	m_ePreState = STATE_IDLE;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(12.f, 0.f, 17.f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scaled(XMVectorSet(0.75f, 0.75f, 0.75f, 0.f));

	return S_OK;
}

void CVoli_Ghost::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bStop == false)
		Check_Loop(fTimeDelta);

	if (m_eState == STATE_MOVE)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= 1.f)
		{
			m_eState = STATE_Q;
			m_pTransformCom->LookAt(XMVectorSet(12.f, 0.f, 17.f, 1.f));
		}
	}

	if (m_eState == STATE_Q && m_pModelCom->Get_Finished())
	{
		m_eState = STATE_IDLE;
	}

	Change_State(fTimeDelta);

	if (m_bStop == false)
		Update_State(fTimeDelta);
}

void CVoli_Ghost::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_Ghost::Render()
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

		_float fAlpha = 0.5f;

		m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		if (i != 7)//포로 메쉬 제외
			m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CVoli_Ghost::SetUp_Components()
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

HRESULT CVoli_Ghost::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CVoli_Ghost::Change_State(_float fTimeDelta)
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
			m_pTransformCom->LookAt(XMVectorSet(12.f, 0.f, 18.f, 1.f));
			m_fMoveSpeed = 4.0f;
			break;
		}
	}
	m_ePreState = m_eState;
}

void CVoli_Ghost::Check_Loop(_float fTimeDelta)
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

	case STATE_Q:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	case STATE_STUN:
		if (m_pModelCom->Get_IsChange() == false)
			m_pModelCom->Check_Looped(fTimeDelta);
		break;

	default:
		break;
	}
}

void CVoli_Ghost::Update_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_IDLE:
		if (m_bStateChange == true)
		{
			if (m_eDoingState == STATE_MOVE)
			{
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
		}

		else
		{
			if (m_bIsState_In == true)
			{
				if (m_pModelCom->Get_Finished() == true)
				{		
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

			if (m_pModelCom->Get_IsChange() == false)
				m_pModelCom->Play_Animation(fTimeDelta);
		}
		break;

	case STATE_MOVE:
		Move(fTimeDelta);
		break;

	case STATE_Q:
		Q_Skill(fTimeDelta);
		break;

	case STATE_STUN:
		Stun(fTimeDelta);
		break;

	default:
		break;
	}
}

void CVoli_Ghost::Move(_float fTimeDelta)
{
	if (m_bAirborne == false)
		m_pTransformCom->Go_Straight(_double(m_fMoveSpeed * fTimeDelta));

	if (m_bStateChange == true)
	{
		if (m_eDoingState == STATE_IDLE)
		{			
			m_iCurrentIndex = 24;
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
			m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVoli_Ghost::Q_Skill(_float fTimeDelta)
{
	if (m_bStateChange == true)
	{
		m_iCurrentIndex = 26;
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
		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CVoli_Ghost::Stun(_float fTimeDelta)
{
}

CVoli_Ghost * CVoli_Ghost::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_Ghost*		pInstance = new CVoli_Ghost(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_Ghost"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_Ghost::Clone(void * pArg)
{
	CVoli_Ghost*		pInstance = new CVoli_Ghost(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_Ghost"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVoli_Ghost::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
