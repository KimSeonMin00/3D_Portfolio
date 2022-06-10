#include "..\Public\HierarchyNode.h"



CHierarchyNode::CHierarchyNode()
{
}

void CHierarchyNode::Set_OffsetMatrix(const _float4x4 * pOffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(pOffsetMatrix)));
}

void CHierarchyNode::Set_TransformationMatrix(_fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);
}

HRESULT CHierarchyNode::NativeConstruct(const char * pBoneName, _float4x4 TransformMatrix, CHierarchyNode * pParent, _uint iDepth)
{
	strcpy_s(m_szName, pBoneName);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransformMatrix)));

	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	m_iDepth = iDepth;

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	_matrix			ParentCombinedMatrix = XMMatrixIdentity();

	if (nullptr != m_pParent)
		ParentCombinedMatrix = m_pParent->Get_CombinedTransformationMatrix();

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * ParentCombinedMatrix);
}

CHierarchyNode * CHierarchyNode::Create(const char * pBoneName, _float4x4 TransformMatrix, CHierarchyNode * pParent, _uint iDepth)
{
	CHierarchyNode*		pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pBoneName, TransformMatrix, pParent, iDepth)))
	{
		MSGBOX(TEXT("Failed to Created : CHierarchyNode"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}
