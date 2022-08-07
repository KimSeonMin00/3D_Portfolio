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
class CPantheon_Hit_Effect
	:public CGameObject
{
protected:
	explicit CPantheon_Hit_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_Hit_Effect(const  CPantheon_Hit_Effect& rhs);
	virtual ~CPantheon_Hit_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT				Render_Slash();
	HRESULT				Render_Flash();

private:
	_float					m_fFlashScale = 0.5f;
	_float					m_fFlashAlpha = 1.f;
	_float					m_fSlashScale = 2.f;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Buffer = nullptr;
	CTexture*						m_pTextureFlash = nullptr;
	CTexture*						m_pTextureSlash = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPantheon_Hit_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
