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
class CHit_Effect_Normal
	:public CGameObject
{
protected:
	explicit CHit_Effect_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CHit_Effect_Normal(const CHit_Effect_Normal& rhs);
	virtual ~CHit_Effect_Normal() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	_float								m_fScale = 0.1f;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

public:
	static CHit_Effect_Normal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
