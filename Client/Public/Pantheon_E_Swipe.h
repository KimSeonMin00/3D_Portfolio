#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CTexture;
END


BEGIN(Client)
class CPantheon_E_Swipe
	:public CGameObject
{
protected:
	explicit CPantheon_E_Swipe(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_E_Swipe(const CPantheon_E_Swipe& rhs);
	virtual ~CPantheon_E_Swipe() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float		m_fLiveTime = 0.f;
	_float      m_fTexMove = 0.f;

	_float      m_fAlpha = 1.f;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModel_Swipe = nullptr;
	CTexture*						m_pTexture_Indicator = nullptr;
	CTexture*						m_pTexture_Fill = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPantheon_E_Swipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
