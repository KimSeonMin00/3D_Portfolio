#pragma once

#include "WhirlWind.h"

class CWhirlWind_Normal final
	:public CWhirlWind
{
public:
	typedef struct tagScaleAlpha
	{
		_float fScale = 0.1f;
		_float fAlpha = 2.f;
		_float fPosY = 0.f;
		_matrix matWorld;
	}SCALEALPHA;

protected:
	explicit CWhirlWind_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CWhirlWind_Normal(const CWhirlWind_Normal& rhs);
	virtual ~CWhirlWind_Normal() = default;

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
	_uint							m_iNumModel = 0;
	vector<SCALEALPHA>				m_vScaleAlpha;
	_float							m_fScale = 0.5f;
	_float							m_fAddMatrixTime = 0.f;


	_vector							m_vMoveDir;
	_float							m_fMoveDist = 0.f;

public:
	static CWhirlWind_Normal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

