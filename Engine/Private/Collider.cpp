#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pAABB(rhs.m_pAABB)
	, m_pOBB(rhs.m_pOBB)
	, m_pSPHERE(rhs.m_pSPHERE)

	, m_pAABB_World(rhs.m_pAABB_World)
	, m_pOBB_World(rhs.m_pOBB_World)
	, m_pSPHERE_World(rhs.m_pSPHERE_World)

#ifdef _DEBUG
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	m_eType = eType;

	switch (eType)
	{
	case TYPE_AABB:
		m_pAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB_World = new BoundingBox();
		break;

	case TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB_World = new BoundingOrientedBox();
		break;

	case TYPE_SPHERE:
		m_pSPHERE = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		m_pSPHERE_World = new BoundingSphere();
		break;
	}

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = nullptr;

	size_t		iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;


#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	COLLIDERDESC		ColliderDesc;
	memcpy(&ColliderDesc, pArg, sizeof(COLLIDERDESC));

	_matrix			TransformMatrix;

	_matrix			ScaleMatrix, RotationXMatrix, RotationYMatrix, RotationZMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);
	RotationXMatrix = XMMatrixRotationX(ColliderDesc.vAngle.x);
	RotationYMatrix = XMMatrixRotationY(ColliderDesc.vAngle.y);
	RotationZMatrix = XMMatrixRotationZ(ColliderDesc.vAngle.z);
	TranslationMatrix = XMMatrixTranslation(ColliderDesc.vPosition.x, ColliderDesc.vPosition.y, ColliderDesc.vPosition.z);

	if (TYPE_OBB == m_eType)
		TransformMatrix = ScaleMatrix * RotationXMatrix * RotationYMatrix * RotationZMatrix * TranslationMatrix;

	else
		TransformMatrix = ScaleMatrix * TranslationMatrix;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB->Transform(*m_pAABB, TransformMatrix);
		break;
	case TYPE_OBB:
		m_pOBB->Transform(*m_pOBB, TransformMatrix);
		break;
	case TYPE_SPHERE:
		m_pSPHERE->Transform(*m_pSPHERE, TransformMatrix);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB->Transform(*m_pAABB_World, Remove_Rotation(WorldMatrix));
		break;
	case TYPE_OBB:
		m_pOBB->Transform(*m_pOBB_World, WorldMatrix);
		break;
	case TYPE_SPHERE:
		m_pSPHERE->Transform(*m_pSPHERE_World, WorldMatrix);
		break;
	}
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	CPipeline*		pPipeline = GET_INSTANCE(CPipeline);

	m_pEffect->SetView(pPipeline->Get_TransformMatrix(CPipeline::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeline->Get_TransformMatrix(CPipeline::D3DTS_PROJ));

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pDeviceContext);



	m_pBatch->Begin();

	switch (m_eType)
	{
	case TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB_World);
		break;
	case TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB_World);
		break;
	case TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSPHERE_World);
		break;
	}

	m_pBatch->End();

	RELEASE_INSTANCE(CPipeline);

	return S_OK;
}
#endif

_matrix CCollider::Remove_Rotation(_fmatrix Transform)
{
	_matrix		Result = Transform;


	_float		fScaleX = XMVectorGetX(XMVector3Length(Transform.r[0]));
	_float		fScaleY = XMVectorGetX(XMVector3Length(Transform.r[1]));
	_float		fScaleZ = XMVectorGetX(XMVector3Length(Transform.r[2]));

	Result.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScaleX;
	Result.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScaleY;
	Result.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScaleZ;

	return Result;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSGBOX(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pAABB);
		Safe_Delete(m_pOBB);
		Safe_Delete(m_pSPHERE);

		Safe_Delete(m_pAABB_World);
		Safe_Delete(m_pOBB_World);
		Safe_Delete(m_pSPHERE_World);

#ifdef _DEBUG	
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
#endif // _DEBUG
	}

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
#endif // _DEBUG
}
