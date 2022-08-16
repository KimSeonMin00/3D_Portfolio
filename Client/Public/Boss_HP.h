#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)
class CBoss_HP
	:public CUI
{
protected:
	CBoss_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	CBoss_HP(const CBoss_HP& rhs);
	virtual ~CBoss_HP() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture* m_pTexture_Frame;
	CTexture* m_pTexture_HP;
private:
	_float		m_fAlpha = 1.f;
	_float		m_fRatio = 1.f;

public:
	void		Set_Ratio(_float fRatio)
	{
		m_fRatio = fRatio;
	}

public:
	static CBoss_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
