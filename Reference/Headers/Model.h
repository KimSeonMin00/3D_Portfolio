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
	virtual HRESULT NativeConstruct_Prototype(const char* pModelFilePath, const char* pModelFileName, TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Render(class CShader* pShader);

private:
	const aiScene*			m_pScene = nullptr;
	Assimp::Importer		m_Importer;
	TYPE					m_eType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private:
	HRESULT Ready_MeshContainers();




public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pModelFilePath, const char* pModelFileName, TYPE eType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

