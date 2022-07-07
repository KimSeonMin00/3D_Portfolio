#pragma once

#include "Effect.h"

class CEffect_Voli_E final:
	public CEffect
{
protected:
	explicit CEffect_Voli_E(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CEffect_Voli_E(const CEffect& rhs);
	virtual ~CEffect_Voli_E() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Texture_Components();

private:
	_float m_fCastingTime = 0.f;
	_float m_fLiveTime = 0.f;

public:
	static  CEffect_Voli_E* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

