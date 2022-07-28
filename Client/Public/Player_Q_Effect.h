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
class CPlayer_Q_Effect final
	:public CGameObject
{
protected:
	explicit  CPlayer_Q_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit  CPlayer_Q_Effect(const  CPlayer_Q_Effect& rhs);
	virtual ~CPlayer_Q_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float							m_fTime = 0.f;
	_float							m_fScale = 1.f;
	_float							m_fAlpha = 1.f;

	_bool							m_bSword = false;
	_bool							m_bCrack = false;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBuffer_RectCom = nullptr;
	CTexture*					m_pTexture_Indicator = nullptr;
	CTexture*					m_pTexture_Sword = nullptr;
	CTexture*					m_pTexture_Crack = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_Q_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
