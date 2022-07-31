#include "stdafx.h"
#include "..\Public\Player_HP.h"
#include "GameInstance.h"

CPlayer_HP::CPlayer_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_HP::CPlayer_HP(const CPlayer_HP & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_HP::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_HP::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_vector	vPos;

	memcpy(&vPos, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scaled(XMVectorSet(m_fScale, m_fScale / 5.f, 1.f, 0.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CPlayer_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_HP::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix WorldMat;
	_matrix ViewMat;

	WorldMat = m_pTransformCom->Get_WorldMatrix();
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

	m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(1.f, 1.f, 1.f, 1.f), sizeof(_vector));

	_float fAlpha = 1.f;
	m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float));

	m_pTexture_Frame->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	m_pTexture_HP->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Set_RawValue("g_Ratio", &m_fRatio, sizeof(_float));

	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CPlayer_HP::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	//Texture
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_HP_Frame"), TEXT("Com_Texture_Frame"), (CComponent**)&m_pTexture_Frame)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Texture_Player_HP"), TEXT("Com_Texture_HP"), (CComponent**)&m_pTexture_HP)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_HP::SetUp_ConstantTable()
{
	return S_OK;
}

CPlayer_HP * CPlayer_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_HP*		pInstance = new CPlayer_HP(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_HP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_HP::Clone(void * pArg)
{
	CPlayer_HP*		pInstance = new CPlayer_HP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_HP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_HP::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTexture_Frame);
	Safe_Release(m_pTexture_HP);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
