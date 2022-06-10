#include "..\Public\MeshContainer.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	:CVIBuffer(rhs)
{
}

void CMeshContainer::Get_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	_uint		iIndex = 0;

	for (auto& pHierarchyNode : m_Bones)
	{
		_matrix		OffsetMatrix = pHierarchyNode->Get_OffsetMatrix();
		_matrix		CombinedMatrix = pHierarchyNode->Get_CombinedTransformationMatrix();

		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(OffsetMatrix * CombinedMatrix * PivotMatrix));
	}
}

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel::TYPE eModelType, aiMesh * pAIMesh, _fmatrix PivotMatrix, CModel* pModel)
{
	HRESULT		hr = 0;

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	if (CModel::TYPE_NONANIM == eModelType)
		hr = Ready_NonAnimModel(pAIMesh, PivotMatrix);
	else
		hr = Ready_AnimModel(pAIMesh, pModel);


#pragma region INDEX_BUFFER
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_iNumIndicesPerPrimitive = 3;
	m_iIndicesStride = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iNumPrimitive * m_iIndicesStride;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Ready_NonAnimModel(aiMesh * pAIMesh, _fmatrix PivotMatrix)
{
	m_iStride = sizeof(VTXNONANIM);
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumVertexBuffer = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXNONANIM*			pVertices = new VTXNONANIM[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix)));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimModel(aiMesh * pAIMesh, CModel* pModel)
{
	m_iStride = sizeof(VTXANIM);
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumVertexBuffer = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXANIM*			pVertices = new VTXANIM[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIM) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	if (FAILED(SetUp_SkinnedDesc(pAIMesh, pVertices, pModel)))
		return E_FAIL;

	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::SetUp_SkinnedDesc(aiMesh * pAIMesh, VTXANIM * pVertices, CModel * pModel)
{
	m_iNumBones = pAIMesh->mNumBones;

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		CHierarchyNode*		pHierarchyNode = pModel->Find_HierarcyNodes(m_szName);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		m_Bones.push_back(pHierarchyNode);

		return S_OK;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pBone = pAIMesh->mBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->Find_HierarcyNodes(pBone->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		_float4x4			OffsetMatrix;
		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

		pHierarchyNode->Set_OffsetMatrix(&OffsetMatrix);

		m_Bones.push_back(pHierarchyNode);

		for (_uint j = 0; j < pBone->mNumWeights; ++j)
		{
			_uint		iVertexIndex = pBone->mWeights[j].mVertexId;

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pBone->mWeights[j].mWeight;
			}

			else
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pBone->mWeights[j].mWeight;
			}
		}
	}

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CModel::TYPE eModelType, aiMesh * pAIMesh, _fmatrix PivotMatrix, CModel* pModel)
{
	CMeshContainer*		pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eModelType, pAIMesh, PivotMatrix, pModel)))
	{
		MSGBOX(TEXT("Failed to Created : CMeshContainer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*		pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CMeshContainer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();
}
