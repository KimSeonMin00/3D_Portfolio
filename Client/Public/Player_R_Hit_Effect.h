#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CPlayer_R_Hit_Effect
	:public CGameObject
{
public:
	typedef struct tagBloodData
	{
		_uint  iIndex = 0;
		_float fScale = 1.f;
		_float fRadian = 0.f;
		_float fSpeed = 0.f;
		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}BLOODDATA;

protected:
	explicit CPlayer_R_Hit_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_R_Hit_Effect(const  CPlayer_R_Hit_Effect& rhs);
	virtual ~CPlayer_R_Hit_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Slash();
	HRESULT Render_Flash();
	HRESULT Render_Blood();

private:
	_float						m_fLiveTime = 0.f;

	_float						m_fSlashRadian;
	_float						m_fSlashAlpha = 1.f;
	_vector						m_vSlashScale;

	_float						m_fFlashAlpha = 1.f;
	_float						m_fFlashScale = 3.f;

	vector<BLOODDATA*>			m_vecBloodData;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRectCom = nullptr;
	CTexture*					m_pTextureSlash = nullptr;
	CTexture*					m_pTextureFlash = nullptr;
	CTexture*					m_pTextureBlood = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iNumModel);

public:
	static CPlayer_R_Hit_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
