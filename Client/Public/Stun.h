#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)
class CStun
	:public CGameObject
{
protected:
	explicit CStun(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CStun(const CStun& rhs);
	virtual ~CStun() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float		m_fTimeAcc = 0.f;
	_float		m_fRadian = 0.f;

public:
	HRESULT SetUp_Components();

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

public:
	static CStun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
