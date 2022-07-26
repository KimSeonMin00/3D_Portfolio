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
class CPlayer_Attack_Effect final
	:public CGameObject
{
public:
	typedef struct tagTranformAlpha
	{
		_float		fAlpha = 1.f;
		_vector		fScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	}TRANSFORMALPHA;

protected:
	explicit CPlayer_Attack_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_Attack_Effect(const  CPlayer_Attack_Effect& rhs);
	virtual ~CPlayer_Attack_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Slash();
	HRESULT Render_Wind();

private:
	TRANSFORMALPHA m_SlashTA;
	TRANSFORMALPHA m_WindTA;

	_bool			m_bFlash = true;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pSlash_Buffer = nullptr;
	CVIBuffer_Rect*				m_pWind_Buffer = nullptr;
	CTexture*					m_pTexture_Slash = nullptr;
	CTexture*					m_pTexture_Wind = nullptr;
	CTexture*					m_pTexture_Flash = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_Attack_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
