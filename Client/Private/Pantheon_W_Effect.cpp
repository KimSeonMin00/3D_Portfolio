#include "stdafx.h"
#include "..\Public\Pantheon_W_Effect.h"
#include "GameInstance.h"
#include "Player.h"

CPantheon_W_Effect::CPantheon_W_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPantheon_W_Effect::CPantheon_W_Effect(const CPantheon_W_Effect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPantheon_W_Effect::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPantheon_W_Effect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_SourColor = XMVectorSet(1.f, 1.f, 100.f / 255.f, 1.f);
	m_DestColor = XMVectorSet(1.f, 50.f / 255.f, 0.f, 1.f);

	for (_int i = 0; i < 12; i++)
	{
		FIREDATA* pFire = new FIREDATA;

		pFire->iIndex = rand() % 16;
		pFire->m_fScale = 4.f;
		pFire->m_vDir = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMMatrixRotationY(XMConvertToRadians(30.f * i)));
		pFire->m_vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		m_vecFire.push_back(pFire);
	}

	return S_OK;
}

void CPantheon_W_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fLiveTime += fTimeDelta;
	if (m_fLiveTime >= 1.f)
		m_bDead = true;

	if (m_fLiveTime >= 0.8f)
		m_fCrackAlpha -= 5.f * fTimeDelta;

	if(m_fSlamX > 0.f)
		m_fSlamX -= 4.f * fTimeDelta;

	for (auto& pFire : m_vecFire)
	{
		pFire->m_fScale -= 4.f * fTimeDelta;
		pFire->m_vPos += pFire->m_vDir * fTimeDelta * 2.f;
	}

	m_pSphere->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_bPlayer_Hit == false)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (((CCollider*)pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Player"), TEXT("Com_HitBox")))->Collision_AABB(m_pSphere))
		{
			((CPlayer*)pGameInstance->Get_GameObjectPtr(m_iLevel, TEXT("Layer_Player")))->Damaged(10.f);

			m_bPlayer_Hit = true;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPantheon_W_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPantheon_W_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_Crack();

	Render_Slam();

	Render_Fire();

	m_pSphere->Render();

	return S_OK;
}

HRESULT CPantheon_W_Effect::Render_Slam()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(m_fSlamX, 4.f, 1.f) *
		XMMatrixTranslation(0.f, 2.f, 0.f) *
		m_pTransformCom->Get_WorldMatrix();
	
	/*_float xScale = XMVectorGetX(XMVector3Length(WorldMat.r[0]));
	_float yScale = XMVectorGetX(XMVector3Length(WorldMat.r[1]));
	_float zScale = XMVectorGetX(XMVector3Length(WorldMat.r[2]));

	ViewMat = pGameInstance->Get_TransformMatrix(CPipeline::D3DTS_VIEW);
	ViewMat = XMMatrixInverse(nullptr, ViewMat);

	WorldMat.r[0] = ViewMat.r[0] * xScale;
	WorldMat.r[1] = ViewMat.r[1] * yScale;
	WorldMat.r[2] = ViewMat.r[2] * zScale;*/


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTexture_Slam->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 100.f / 255.f, 1.f), sizeof(_vector));

	_float fAlpha = 1.f;

	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pShaderCom->Begin(2);

	m_pRect_Com->Render();

	return S_OK;
}

HRESULT CPantheon_W_Effect::Render_Crack()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = XMMatrixIdentity() *
		XMMatrixScaling(3.f, 3.f, 1.f) *
		XMMatrixRotationX(XMConvertToRadians(90.f))*
		XMMatrixTranslation(0.f, 0.2f, 0.f) *
		m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WorldMat), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	m_pTexture_Crack->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorLerp(m_SourColor, m_DestColor, m_fLiveTime / 0.5f), sizeof(_vector));

	m_pShaderCom->Set_RawValue("g_Alpha", &m_fCrackAlpha, sizeof(_float));

	m_pShaderCom->Begin(0);

	m_pRect_Com->Render();

	return S_OK;
}

HRESULT CPantheon_W_Effect::Render_Fire()
{
	for (auto& pFire : m_vecFire)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_matrix WorldMat;
		_matrix ViewMat;
		_float3 vPos;
		XMStoreFloat3(&vPos, pFire->m_vPos);

		WorldMat = XMMatrixIdentity() *
			XMMatrixScaling(pFire->m_fScale, pFire->m_fScale, 1.f) *
			XMMatrixTranslation(vPos.x, vPos.y, vPos.z) *
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

		m_pTexture_Fire->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", pFire->iIndex);

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorLerp(m_SourColor, m_DestColor, m_fLiveTime / 0.5f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_fCrackAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		m_pRect_Com->Render();
	}

	return E_NOTIMPL;
}

HRESULT CPantheon_W_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pRect_Com)))
		return E_FAIL;

	//Texture
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_W_Slam"), TEXT("Com_Texture_Slam"), (CComponent**)&m_pTexture_Slam)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_W_Crack"), TEXT("Com_Texture_Crack"), (CComponent**)&m_pTexture_Crack)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Pantheon_Fire"), TEXT("Com_Texture_Fire"), (CComponent**)&m_pTexture_Fire)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Hit_Sphere"), (CComponent**)&m_pSphere, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CPantheon_W_Effect * CPantheon_W_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPantheon_W_Effect*		pInstance = new CPantheon_W_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPantheon_W_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPantheon_W_Effect::Clone(void * pArg)
{
	CPantheon_W_Effect*		pInstance = new  CPantheon_W_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned :  CPantheon_W_Effect"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPantheon_W_Effect::Free()
{
	__super::Free();

	for (auto& pFire : m_vecFire)
	{
		Safe_Delete(pFire);
	}
	m_vecFire.clear();


	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRect_Com);
	Safe_Release(m_pTexture_Slam);
	Safe_Release(m_pTexture_Crack);
	Safe_Release(m_pShaderCom);
}
