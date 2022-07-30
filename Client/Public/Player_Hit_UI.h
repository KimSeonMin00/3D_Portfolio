#pragma once
#include "UI.h"

BEGIN(Client)
class CPlayer_Hit_UI
	:public CUI
{
protected:
	CPlayer_Hit_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	CPlayer_Hit_UI(const CPlayer_Hit_UI& rhs);
	virtual ~CPlayer_Hit_UI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float		m_fAlpha = 1.f;

public:
	static CPlayer_Hit_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END