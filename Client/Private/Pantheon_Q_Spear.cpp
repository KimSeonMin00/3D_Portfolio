#include "stdafx.h"
#include "..\Public\Pantheon_Q_Spear.h"
#include "GameInstance.h"
#include "Player.h"

CPantheon_Q_Spear::CPantheon_Q_Spear(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_Q_Spear::CPantheon_Q_Spear(const CPantheon_Q_Spear & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_Q_Spear::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Q_Spear::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;



	if (pArg != nullptr)
	{
		_matrix  WorldMat;

		memcpy(&WorldMat, pArg, sizeof(_matrix));

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMat.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMat.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMat.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3] + XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));

		m_RectMatrix.r[0] = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		m_RectMatrix.r[1] = -XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		m_RectMatrix.r[2] = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_RectMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPantheon_Q_Spear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	

	if (m_bStop == false)
	{
		m_pTransformCom->Go_Direction(m_pTransformCom->Get_State(CTransform::STATE_UP), 30.f * fTimeDelta);
		m_RectMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_fTexMove += 2.f * fTimeDelta;
		m_fMoveDist += 30.f * fTimeDelta;
		if (m_fMoveDist >= 10.f)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			m_bStop = true;
		}
	}
	else
	{
		m_fLiveTime += fTimeDelta;
		if (m_fLiveTime >= 1.f)
		{
			m_bDead = true;
		}

		else if(m_fLiveTime >= 0.5f)
		{
			m_pTransformCom->Go_Direction(XMVectorSet(0.f, -1.f, 0.f, 0.f), 2.f * fTimeDelta);
		}
	}

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pSphereCom))
	{
		((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

		CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

		_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

		pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Pantheon_Q_Hit_Effect"), &vPlayerPos);
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CPantheon_Q_Spear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_Q_Spear::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModel_Spear)

		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_float fAlpha = 1.f;

	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pModel_Spear->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", 0, aiTextureType_DIFFUSE);

	m_pShaderCom->Begin(0);

	m_pModel_Spear->Render(0);

	if (m_bStop == false)
	{
		Render_Spear();

		Render_Trail();
	}

	m_pSphereCom->Render();

	return S_OK;
}

HRESULT CPantheon_Q_Spear::Render_Trail()
{
	if (nullptr == m_pShader_Rect ||
		nullptr == m_pRectCom)

		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix RectMat = m_RectMatrix;
	RectMat.r[3] += -3.f * m_pTransformCom->Get_State(CTransform::STATE_UP);
	
	WorldMat = XMMatrixIdentity()*
		XMMatrixScaling(4.f, 0.8f, 1.f)*
		RectMat;

	m_pShader_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	_float2 MoveTex = _float2(m_fTexMove, 0.f);

	m_pShader_Rect->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	_float fAlpha = 1.f;

	m_pShader_Rect->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShader_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pTextureTrail->Bind_OnShader(m_pShader_Rect, "g_DiffuseTexture");

	m_pShader_Rect->Begin(5);

	m_pRectCom->Render();

	WorldMat = XMMatrixIdentity()*
		XMMatrixScaling(4.f, 1.2f, 1.f)*
		RectMat;
	m_pShader_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));

	m_pTextureTrailCenter->Bind_OnShader(m_pShader_Rect, "g_DiffuseTexture");

	m_pShader_Rect->Begin(5);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPantheon_Q_Spear::Render_Spear()
{
	if (nullptr == m_pShader_Rect ||
		nullptr == m_pRectCom)

		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity()*
		XMMatrixScaling(2.f, 1.f, 1.f) *
		m_RectMatrix;

	m_pShader_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	_float2 MoveTex = _float2(m_fTexMove, 0.f);

	m_pShader_Rect->Set_RawValue("g_vMoveTex", &MoveTex, sizeof(_float2));

	_float fAlpha = 1.f;

	m_pShader_Rect->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShader_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 100.f/255.f, 1.f), sizeof(_vector));

	m_pTextureSpear->Bind_OnShader(m_pShader_Rect, "g_DiffuseTexture");

	m_pShader_Rect->Begin(2);

	m_pRectCom->Render();

	WorldMat = XMMatrixIdentity()*
		XMMatrixScaling(4.f, 2.f, 1.f) *
		m_RectMatrix;

	m_pShader_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShader_Rect->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pShader_Rect->Set_RawValue("g_vColor", &XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f), sizeof(_vector));

	m_pTextureBlur->Bind_OnShader(m_pShader_Rect, "g_DiffuseTexture");

	m_pShader_Rect->Begin(2);

	m_pRectCom->Render();

	return S_OK;
}

HRESULT CPantheon_Q_Spear::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShader_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pRectCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Pantheon_Q_Spear"), TEXT("Com_Model_Spear"), (CComponent**)&m_pModel_Spear)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Trail"), TEXT("Com_Texture_Trail"), (CComponent**)&m_pTextureTrail)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Trail_Center"), TEXT("Com_Texture_Trail_Center"), (CComponent**)&	m_pTextureTrailCenter)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear"), TEXT("Com_Texture_Spear"), (CComponent**)&m_pTextureSpear)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear_Blur"), TEXT("Com_Texture_Blur"), (CComponent**)&m_pTextureBlur)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Hit_Sphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_Q_Spear::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPantheon_Q_Spear * CPantheon_Q_Spear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_Q_Spear*		pInstance = new CPantheon_Q_Spear(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_Q_Spear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_Q_Spear::Clone(void * pArg)
{
	CPantheon_Q_Spear*		pInstance = new CPantheon_Q_Spear(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPantheon_Q_Spear"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_Q_Spear::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel_Spear);
	Safe_Release(m_pShaderCom);
}
