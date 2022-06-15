#pragma once
#include "Client_Defines.h"
#include "Monster.h"

class CVolibear
	final : public CMonster
{
protected:
	explicit CVolibear(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVolibear(const CVolibear& rhs);
	virtual ~CVolibear() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


private:
	_float	m_fTimeAcc = 0.f;

	_uint	m_iIndex = 0;
public:
	static CVolibear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

