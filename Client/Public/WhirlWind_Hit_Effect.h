#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)
class CWhirlWind_Hit_Effect
	:public CGameObject
{
public:
	typedef struct tagWindData
	{
		_float fScale = 0.5f;
		_float fAlpha = 1.f;
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}WINDDATA;

	typedef struct tagSmokeData
	{
		_uint  iIndex = 0;
	}SMOKEDATA;

protected:
	explicit CWhirlWind_Hit_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CWhirlWind_Hit_Effect(const CWhirlWind_Hit_Effect& rhs);
	virtual ~CWhirlWind_Hit_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Wind();
	HRESULT Render_Smoke();
	HRESULT Render_Flash();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iNumModel);

private:
	_float							m_fLiveTime = 0.f;

	_float							m_fAddWindTime = 0.f;
	_float							m_fSmokeScale = 1.f;
	_float							m_fSmokeAlpha = 1.f;

	_vector							m_vFlashScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	_vector							m_vFlashScaleOrigin;
	_float							m_fFlashAlpha = 1.f;

	vector<WINDDATA*>				m_vecWind;
	vector<SMOKEDATA*>				m_vecSmoke;

private:
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CShader*						m_pShaderCom_Rect = nullptr;
	CShader*						m_pShaderCom_Model = nullptr;
	CVIBuffer_Rect*					m_pRectCom = nullptr;
	CTexture*						m_pTextureWind = nullptr;
	CTexture*						m_pTextureFlash = nullptr;
	CTexture*						m_pTextureSmoke = nullptr;


public:
	static CWhirlWind_Hit_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
