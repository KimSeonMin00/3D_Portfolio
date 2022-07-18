#pragma once

#include "WhirlWind.h"

class CWhirlWind_EQ final
	:public CWhirlWind
{
public:
	typedef struct tagE_Q_Data
	{
		_float fScale = 1.f;
		_float fAlpha = 1.f;
		_bool  bWhite = false;
		_bool  bTurn = false;
	}EQDATA;

protected:
	explicit CWhirlWind_EQ(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CWhirlWind_EQ(const CWhirlWind_EQ& rhs);
	virtual ~CWhirlWind_EQ() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iNumModel);

private:
	CTexture*						m_pTexture_White = nullptr;
	vector<EQDATA*>					m_vecEqData;
	_uint							m_iModel = 0;
	_float							m_fAddModelTime = 0.f;
	
private:
	_float							m_fLiveTime = 0.f;

public:
	static CWhirlWind_EQ* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

