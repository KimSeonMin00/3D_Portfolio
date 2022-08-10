#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CVoli_Q_Down
	:public CGameObject
{
protected:
	explicit CVoli_Q_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVoli_Q_Down(const CVoli_Q_Down& rhs);
	virtual ~CVoli_Q_Down() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT			Render_Gust();
	HRESULT			Render_Crack();

private:
	_float					    m_fLiveTime = 0.f;

	_float						m_fGustScale = 10.f;
	_float						m_fGustAlpha = 1.f;

	_float						m_fCrackAlpha = 1.f;

	_bool						m_bPlayer_Hit = false;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Buffer = nullptr;
	CTexture*					m_pTextureDownGust = nullptr;
	CTexture*					m_pTextureCrack = nullptr;
	CCollider*					m_pSphereCom = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static CVoli_Q_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
