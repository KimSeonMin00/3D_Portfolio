#include "stdafx.h"
#include "..\Public\Voli_Q_Down.h"
#include "GameInstance.h"
#include "Player.h"

CVoli_Q_Down::CVoli_Q_Down(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CVoli_Q_Down::CVoli_Q_Down(const CVoli_Q_Down & rhs)
	:CGameObject(rhs)
{
}

HRESULT CVoli_Q_Down::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVoli_Q_Down::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CVoli_Q_Down::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fGustScale > 0.f)
	{
		m_fGustScale -= 20.f * fTimeDelta;
	}
	else
	{
		m_fCrackAlpha -= 1.f * fTimeDelta;

		if (m_fCrackAlpha <= 0.f)
			m_bDead = true;
	}

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_bPlayer_Hit == false)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pSphereCom))
		{
			((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(100.f);

			CTransform* pPlayer_Transform = (CTransform*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_Transform"));

			_vector vPlayerPos = pPlayer_Transform->Get_State(CTransform::STATE_POSITION);

			pGameInstance->Add_Layer(m_iLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Voli_Hit"), &vPlayerPos);

			m_bPlayer_Hit = true;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CVoli_Q_Down::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CVoli_Q_Down::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Crack();

	Render_Gust();

	return S_OK;
}

HRESULT CVoli_Q_Down::Render_Gust()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(m_fGustScale, m_fGustScale, 1.f) *
		m_pTransformCom->Get_WorldMatrix();

	_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureDownGust->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fGustAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CVoli_Q_Down::Render_Crack()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;

	WorldMat = XMMatrixIdentity() * XMMatrixScaling(4.f, 4.f, 1.f) * 
		XMMatrixRotationX(XMConvertToRadians(90.f)) * 
		XMMatrixTranslation(0.f, 0.2f, 0.f) * 
		m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTextureCrack->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fCrackAlpha, sizeof(_float));

	m_pShaderCom->Begin(0);

	m_pRect_Buffer->Render();

	return S_OK;
}

HRESULT CVoli_Q_Down::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Buffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Flash"), TEXT("Com_Texture_Downgust"), (CComponent**)&m_pTextureDownGust)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Voli_Q_Crack"), TEXT("Com_Texture_Crack"), (CComponent**)&m_pTextureCrack)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Hit_Sphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CVoli_Q_Down * CVoli_Q_Down::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CVoli_Q_Down*		pInstance = new CVoli_Q_Down(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CVoli_Q_Down"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVoli_Q_Down::Clone(void * pArg)
{
	CVoli_Q_Down*		pInstance = new CVoli_Q_Down(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVoli_Q_Down"));
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

void CVoli_Q_Down::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Buffer);
	Safe_Release(m_pTextureDownGust);
	Safe_Release(m_pTextureCrack);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
}
