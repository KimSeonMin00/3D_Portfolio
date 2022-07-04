#pragma once

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void Get_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

public:
	virtual HRESULT NativeConstruct_Prototype(CModel::TYPE eModelType, aiMesh* pAIMesh, _fmatrix PivotMatrix, class CModel* pModel);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Ready_NonAnimModel(aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_AnimModel(aiMesh* pAIMesh, class CModel* pModel);
	HRESULT SetUp_HierarchyNodes(class CModel* pModel);

private:
	char			m_szName[MAX_PATH] = "";
	_uint			m_iMaterialIndex = 0;

private:
	_uint									m_iNumBones = 0;
	vector<class CHierarchyNode*>			m_Bones;

private:
	HRESULT SetUp_SkinnedDesc(aiMesh* pAIMesh, VTXANIM* pVertices, class CModel* pModel);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel::TYPE eModelType, aiMesh* pAIMesh, _fmatrix PivotMatrix, class CModel* pModel);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

