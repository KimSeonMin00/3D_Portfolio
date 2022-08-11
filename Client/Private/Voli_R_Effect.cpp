#include "stdafx.h"
#include "..\Public\Voli_R_Effect.h"
#include "GameInstance.h"
#include "Player.h"

CVoli_R_Effect::CVoli_R_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CVoli_R_Effect::CVoli_R_Effect(const CVoli_R_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CVoli_R_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_R_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	vPos = XMVectorSetY(vPos, 0.3f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scaled(XMVectorSet(4.f, 4.f, 1.f, 0.f));
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f) / XMConvertToRadians(60.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CVoli_R_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;
	if (m_fLiveTime <= 1.f)
	{
		if(m_fAlpha < 1.f)
			m_fAlpha += 2.f * fTimeDelta;
	}

	else if (m_fLiveTime > 1.f)
	{
		if (m_bCast == false)
		{
			m_bPlayer_Hit = false;
			m_bCast = true;
		}

		m_fAlpha -= 1.f * fTimeDelta;
	}

	if (m_fLiveTime >= 2.f)
		m_bDead = true;

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_bPlayer_Hit == false)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pSphereCom))
		{
			((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

			CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

			_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Voli_Hit"), &vPlayerPos);

			m_bPlayer_Hit = true;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CVoli_R_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_R_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if(m_fLiveTime <= 1.f)
		Render_Warning();
	else
		Render_Crator();

	return S_OK;
}

HRESULT CVoli_R_Effect::Render_Warning()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() * 
		m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureWarning->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin(6);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CVoli_R_Effect::Render_Crator()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() * 
		m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureCrator->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin(6);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CVoli_R_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_R_Warning"), TEXT("Com_Texture_Warning"), (CComponent**)&m_pTextureWarning)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_R_Crator"), TEXT("Com_Texture_Crator"), (CComponent**)&m_pTextureCrator)))
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

CVoli_R_Effect * CVoli_R_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_R_Effect*		pInstance = new CVoli_R_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_R_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_R_Effect::Clone(void * pArg)
{
	CVoli_R_Effect*		pInstance = new CVoli_R_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_R_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

void CVoli_R_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureWarning);
	Safe_Release(m_pTextureCrator);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
}
