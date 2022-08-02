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
class CPantheon_E_Slash
	:public CGameObject
{
protected:
	explicit CPantheon_E_Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_E_Slash(const CPantheon_E_Slash& rhs);
	virtual ~CPantheon_E_Slash() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float   m_fLiveTime = 0.f;
	_float   m_fScale = 1.f;

public:
	HRESULT SetUp_Components();

private:
	CRenderer*						m_pRendererCom = nullptr;
	CShader*						m_pShaderCom_Rect = nullptr;
	CVIBuffer_Rect*					m_pRectCom = nullptr;
	CTexture*						m_pTextureSlash = nullptr;

public:
	static CPantheon_E_Slash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
