#pragma once

#include "Client_Defines.h"
#include "MapObject_Anim.h"

BEGIN(Client)
class CDuck
	:public CMapObject_Anim
{
protected:
	explicit CDuck(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CDuck(const CDuck& rhs);
	virtual ~CDuck() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static CDuck* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
