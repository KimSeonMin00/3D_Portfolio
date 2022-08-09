#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CVoli_R_Effect
	:public CGameObject
{
protected:
	explicit CVoli_R_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVoli_R_Effect(const CVoli_R_Effect& rhs);
	virtual ~CVoli_R_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT			Render_Warning();
	HRESULT			Render_Crator();

private:
	_float				m_fAlpha = 0.f;

private:
	_float					    m_fLiveTime = 0.f;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Buffer = nullptr;
	CTexture*					m_pTextureWarning= nullptr;
	CTexture*					m_pTextureCrator = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static CVoli_R_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
