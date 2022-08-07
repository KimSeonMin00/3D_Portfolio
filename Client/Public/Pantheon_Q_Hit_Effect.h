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
class CPantheon_Q_Hit_Effect
	:public CGameObject
{
protected:
	explicit CPantheon_Q_Hit_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_Q_Hit_Effect(const CPantheon_Q_Hit_Effect& rhs);
	virtual ~CPantheon_Q_Hit_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT				Render_Flash();
	HRESULT				Render_Flare();
	HRESULT				Render_Cross();
	HRESULT				Render_Spark();

private:
	_float					m_fFlashScale = 0.5f;
	_float					m_fFlashAlpha = 1.f;
	_float					m_fFlareScale = 0.5f;
	_float					m_fFlareAlpha = 1.f;
	_float					m_fCrossScale = 1.f;
	_float					m_fSparkScale = 0.5f;
	_float					m_fSparkAlpha = 1.f;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Buffer = nullptr;
	CTexture*						m_pTextureFlash = nullptr;
	CTexture*						m_pTextureFlare = nullptr;
	CTexture*						m_pTextureCross = nullptr;
	CTexture*						m_pTextureSpark = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static CPantheon_Q_Hit_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
