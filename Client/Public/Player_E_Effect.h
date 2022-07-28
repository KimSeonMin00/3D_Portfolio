#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CPlayer_E_Effect
	:public CGameObject
{
public:
	typedef struct tagTranformAlpha
	{
		_uint		iIndex = 0;
		_float		fScale = 1.f;
	}TRANSFORMALPHA;

protected:
	explicit  CPlayer_E_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit  CPlayer_E_Effect(const  CPlayer_E_Effect& rhs);
	virtual ~CPlayer_E_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Flash();
	HRESULT Render_Ring();

private:
	_float						m_fAlpha = 1.f;
	_float						m_fScale = 3.f;
	_float						m_fAddTime = 0.f;
	vector<TRANSFORMALPHA*>		m_vecTARing;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pFlash_Buffer = nullptr;
	CVIBuffer_Rect*				m_pRing_Buffer = nullptr;
	CTexture*					m_pTexture_Flash = nullptr;
	CTexture*					m_pTexture_Ring = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_E_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
