#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CPlayer_Q_Hit_Effect
	:public CGameObject
{
public:
	typedef struct tagTranformAlpha
	{
		_float		fScale = 1.f;
		_float      fRadian = 0.f;
		_float		fSpeed = 1.f;
		_vector		vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector		vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}TRANSFORMALPHA;

protected:
	explicit CPlayer_Q_Hit_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_Q_Hit_Effect(const  CPlayer_Q_Hit_Effect& rhs);
	virtual ~CPlayer_Q_Hit_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Render_Flash();
	HRESULT	Render_Tar();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pFlash_Buffer = nullptr;
	CVIBuffer_Rect*				m_pFlash_Tar_Buffer = nullptr;
	CTexture*					m_pTexture_Flash = nullptr;
	CTexture*					m_pTexture_Flash_Tar = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	vector<TRANSFORMALPHA*>		m_vecTA;

	_bool		m_bFlash = true;
	_float		m_fFlashAlpha = 0.1f;
	_float		m_fScale = 2.f;

	_float		m_fLiveTime = 0.f;


public:
	static CPlayer_Q_Hit_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END

