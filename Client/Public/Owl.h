#pragma once
#include "Client_Defines.h"
#include "MapObject_Anim.h"

BEGIN(Client)
class COwl
	:public CMapObject_Anim
{
protected:
	explicit COwl(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit COwl(const COwl& rhs);
	virtual ~COwl() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static COwl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
