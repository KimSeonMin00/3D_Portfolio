#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final
	:public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_bool	Get_Finished();
	_uint	Get_KeyFrame();
	_bool	Get_IsChange()
	{
		return m_bIsChange;
	};

public:
	virtual HRESULT NativeConstruct_Prototype(const char* pModelFilePath, const char* pModelFileName, TYPE eType, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT SetUp_Material_OnShader(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT SetUp_AnimationIndex(_uint iAnimIndex);
	HRESULT SetUp_BoneMatrices_OnShader(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void Check_Looped(_double TimeDelta);
	void Play_Animation(_double TimeDelta);
	void Change_Animation(_double TimeDelta, _uint iNextAnimationIndex, _double	m_Duration);

public:
	class CHierarchyNode* Find_HierarcyNodes(const char* pBoneName);

private:
	const aiScene*			m_pScene = nullptr;
	Assimp::Importer		m_Importer;
	TYPE					m_eType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private:
	_float4x4				m_PivotMatrix;

private:
	_uint									m_iNumMaterials = 0;
	vector<MODELMATERIAL>					m_Materials;
	typedef vector<MODELMATERIAL>			MATERIALS;

private:
	vector<CHierarchyNode*>					m_HierarchyNodes;
	typedef vector<CHierarchyNode*>			HIERARCHYNODES;

private:
	_uint									m_iCurrentAnimIndex = 0;
	_uint									m_iNumAnimations = 0;
	vector<class CAnimation*>				m_Animations;
	typedef vector<class CAnimation*>		ANIMATIONS;

private:
	_double			m_TimeAcc = 0.0;
	_double			m_Duration = 0.0;

	_bool			m_bIsChange = false;

public:
	void		Set_Initialize();
	


private:
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animation();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pModelFilePath, const char* pModelFileName, TYPE eType, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

