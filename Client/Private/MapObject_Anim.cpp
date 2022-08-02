#include "stdafx.h"
#include "..\Public\MapObject_Anim.h"
#include "GameInstance.h"

CMapObject_Anim::CMapObject_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CMapObject_Anim::CMapObject_Anim(const CMapObject_Anim & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMapObject_Anim::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject_Anim::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CMapObject_Anim::Tick(_float fTimeDelta)
{
	if (m_pModelCom->Get_IsChange() == false)
		m_pModelCom->Check_Looped(fTimeDelta);

	if (m_pModelCom->Get_IsChange() == false)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CMapObject_Anim::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMapObject_Anim::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);


		if (m_bSelected == false)
		{
			m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));
			m_pShaderCom->Begin(0);
		}

		else
		{
			m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_vector));
			m_pShaderCom->Begin(1);
		}

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject_Anim::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject_Anim::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CMapObject_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
