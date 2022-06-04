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
	virtual HRESULT NativeConstruct_Prototype(CModel::TYPE eModelType, aiMesh* pAIMesh);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Ready_NonAnimModel(aiMesh* pAIMesh);
	HRESULT Ready_AnimModel(aiMesh* pAIMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel::TYPE eModelType, aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

