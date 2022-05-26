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

class CBackGround final :
	public CGameObject
{
public:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

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

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

public:
	HRESULT SetUp_Components();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
