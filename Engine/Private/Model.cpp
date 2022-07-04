#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Shader.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Animations(rhs.m_Animations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_AddRef(pHierarchyNode);

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pTexture)
			Safe_AddRef(pTexture);
	}
}

HRESULT CModel::SetUp_Material_OnShader(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Bind_OnShader(pShader, pConstantName, 0);

	return S_OK;
}

HRESULT CModel::SetUp_AnimationIndex(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	m_iCurrentAnimIndex = iAnimIndex;

	return S_OK;
}

HRESULT CModel::SetUp_BoneMatrices_OnShader(CShader * pShader, const char * pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4		BoneMatrices[180];

	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 180);

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (FAILED(pShader->Set_RawValue(pConstantName, BoneMatrices, sizeof(_float4x4) * 180)))
		return E_FAIL;

	return S_OK;
}

void CModel::Check_Looped(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Check_Looped(TimeDelta);
}

void CModel::Play_Animation(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(TimeDelta);

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

void CModel::Change_Animation(_double TimeDelta, _uint iNextAnimationIndex, _double	m_Duration)
{
	if (m_bIsChange == false)
	{
		m_Animations[iNextAnimationIndex]->Set_Initialize();
		m_TimeAcc = 0.0;
		m_bIsChange = true;
	}

	m_TimeAcc += 24.0 * TimeDelta;

	if (m_Duration <= m_TimeAcc)
	{
		m_iCurrentAnimIndex = iNextAnimationIndex;
		m_bIsChange = false;
		return;
	}
	_vector		vScale, vRotation, vPosition;

	for (_uint i = 0; i < m_Animations[m_iCurrentAnimIndex]->Get_NumChannels(); i++)
	{
		KEYFRAME* pCurrentKeyFrame = m_Animations[m_iCurrentAnimIndex]->Get_CurrentKeyFrameInfo(i);
		if (pCurrentKeyFrame == nullptr)
			return;
		KEYFRAME* pNextKeyFrame = m_Animations[iNextAnimationIndex]->Get_CurrentKeyFrameInfo(i);
		if (pNextKeyFrame == nullptr)
			return;
		CHierarchyNode* pHierarchyNode = m_Animations[m_iCurrentAnimIndex]->Get_Bone(i);
		if (pHierarchyNode == nullptr)
			return;

		Safe_AddRef(pHierarchyNode);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		_double		Ratio = m_TimeAcc/ m_Duration;

		vSourScale = XMLoadFloat3(&pCurrentKeyFrame->vScale);
		vSourRotation = XMLoadFloat4(&pCurrentKeyFrame->vRotation);
		vSourPosition = XMLoadFloat3(&pCurrentKeyFrame->vPosition);

		vDestScale = XMLoadFloat3(&pNextKeyFrame->vScale);
		vDestRotation = XMLoadFloat4(&pNextKeyFrame->vRotation);
		vDestPosition = XMLoadFloat3(&pNextKeyFrame->vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pHierarchyNode->Set_TransformationMatrix(TransformationMatrix);

		Safe_Release(pHierarchyNode);		
	}

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

CHierarchyNode * CModel::Find_HierarcyNodes(const char * pBoneName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pBoneName);
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

_bool CModel::Get_Finished()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
}

_uint CModel::Get_KeyFrame()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_KeyFrame();
}


HRESULT CModel::NativeConstruct_Prototype(const char * pModelFilePath, const char * pModelFileName, TYPE eType, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char	szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	m_eType = eType;

	_uint		iFlag = 0;

	if (TYPE_NONANIM == m_eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
	if (nullptr == m_pScene)
		return E_FAIL;

	if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	//for (auto& pMeshContainer : m_Meshes)
	//{
	//	pMeshContainer->SetUp_HierarchyNodes(this);
	//}

	if (FAILED(Clone_MeshContainer()))
		return E_FAIL;

	if (FAILED(Clone_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (nullptr != m_Meshes[iMeshIndex])
	{
		m_Meshes[iMeshIndex]->Render();
	}

	return S_OK;
}

void CModel::Set_Initialize()
{
		m_Animations[m_iCurrentAnimIndex]->Set_Initialize();
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshes = m_pScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pScene->mMeshes[i];

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eType, pAIMesh, XMLoadFloat4x4(&m_PivotMatrix), this);
		if (nullptr == pMeshContainer)
			return FALSE;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*		pAIMaterial = m_pScene->mMaterials[i];

		MODELMATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		StrPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &StrPath)))
				continue;

			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(StrPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char			szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szTextureFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFullPath, MAX_PATH);

			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szTextureFullPath);
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	_float4x4		TransformMatrix;
	memcpy(&TransformMatrix, &pNode->mTransformation, sizeof(_float4x4));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformMatrix, pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		if (FAILED(Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Clone_MeshContainer()
{
	vector<CMeshContainer*>			Meshes;

	for (auto& pPrototype : m_Meshes)
	{
		CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone(this);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		Meshes.push_back(pMeshContainer);

		Safe_Release(pPrototype);
	}

	m_Meshes.clear();
	m_Meshes = Meshes;

	return S_OK;
}

HRESULT CModel::Clone_Animation()
{
	ANIMATIONS		Anims;

	for (auto& pAnimation : m_Animations)
	{
		if (nullptr != pAnimation)
		{
			CAnimation*		pAnim = pAnimation->Clone_Animation(this);
			if (nullptr != pAnim)
				Anims.push_back(pAnim);

			Safe_Release(pAnimation);
		}
	}
	m_Animations.clear();
	m_Animations = Anims;

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pModelFilePath, const char * pModelFileName, TYPE eType, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModelFilePath, pModelFileName, eType, PivotMatrix)))
	{
		MSGBOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNodes : m_HierarchyNodes)
		Safe_Release(pHierarchyNodes);
	m_HierarchyNodes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pTexture)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	m_Importer.FreeScene();
}
