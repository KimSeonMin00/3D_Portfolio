#pragma once
#include "Client_Defines.h"
#include "MapObject_Anim.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)
class CBird
	:public CMapObject_Anim
{
protected:
	explicit CBird(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CBird(const CBird& rhs);
	virtual ~CBird() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static CBird* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
