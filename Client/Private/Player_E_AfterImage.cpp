#include "stdafx.h"
#include "..\Public\Player_E_AfterImage.h"
#include "GameInstance.h"

CPlayer_E_AfterImage::CPlayer_E_AfterImage(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	:CGameObject(pDevice, pDevice_Context)
{
}

CPlayer_E_AfterImage::CPlayer_E_AfterImage(const CPlayer_E_AfterImage & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer_E_AfterImage::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_E_AfterImage::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_matrix WorldMat;

	memcpy(&WorldMat, pArg, sizeof(_matrix));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMat.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMat.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMat.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_E_AfterImage::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAlpha -= 4.f * fTimeDelta;

	if (m_fAlpha <= 0.f)
		m_bDead = true;
}

void CPlayer_E_AfterImage::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pRendererCom == nullptr)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHABLEND, this);
	
}

HRESULT CPlayer_E_AfterImage::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_AlphaTexture", i, aiTextureType_DIFFUSE);

		_float2 vMoveTex = _float2(0.f, 0.f);

		m_pShaderCom->Set_RawValue("g_vMoveTex", &vMoveTex, sizeof(_float2));

		m_pShaderCom->Set_RawValue("g_vColor", &XMVectorSet(0.f, 1.f, 1.f, 1.f), sizeof(_vector));

		m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CPlayer_E_AfterImage::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(m_iLevel, TEXT("Prototype_Component_Model_Player_E_AF"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
}

HRESULT CPlayer_E_AfterImage::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);
}

CPlayer_E_AfterImage * CPlayer_E_AfterImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CPlayer_E_AfterImage*		pInstance = new CPlayer_E_AfterImage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer_E_AfterImage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_E_AfterImage::Clone(void * pArg)
{
	CPlayer_E_AfterImage*		pInstance = new CPlayer_E_AfterImage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer_E_AfterImage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_E_AfterImage::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
