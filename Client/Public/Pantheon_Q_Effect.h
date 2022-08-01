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
class CPantheon_Q_Effect
	:public CGameObject
{
protected:
	explicit CPantheon_Q_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_Q_Effect(const CPantheon_Q_Effect& rhs);
	virtual ~CPantheon_Q_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Spear();
	HRESULT Render_Trail();

private:
	_float   m_fLiveTime = 0.f;

	_float   m_fScaleX = 4.f;
	_float   m_fScaleY = 2.f;

	_float   m_fTrailAlpha = 1.f;

	_vector m_SourColor;
	_vector m_DestColor;

public:
	HRESULT SetUp_Components();

private:
	CRenderer*						m_pRendererCom = nullptr;
	CShader*						m_pShaderCom_Rect = nullptr;
	CVIBuffer_Rect*					m_pRectCom = nullptr;
	CTexture*						m_pTextureSpear = nullptr;
	CTexture*						m_pTextureSpearBlur = nullptr;
	CTexture*						m_pTextureTrail = nullptr;


public:
	static CPantheon_Q_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
