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

class CFadeOut
	:public CGameObject
{
protected:
	CFadeOut(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	CFadeOut(const CFadeOut& rhs);
	virtual ~CFadeOut() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;

public:
	_bool				Get_FadeOut()
	{
		return m_bFadeOut;
	}

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;
	_float				m_fAlpha = 0.f;
	_float				m_fTimeAcc = 0.f;

	_bool				m_bFadeOut = false;

public:
	HRESULT SetUp_Components();

public:
	static CFadeOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
