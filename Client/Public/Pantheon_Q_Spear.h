#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CPantheon_Q_Spear
	:public CGameObject
{
protected:
	explicit CPantheon_Q_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_Q_Spear(const CPantheon_Q_Spear& rhs);
	virtual ~CPantheon_Q_Spear() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Render_Trail();
	HRESULT Render_Spear();

private:
	_float		m_fLiveTime = 0.f;

	_matrix		m_RectMatrix;
	_float      m_fTexMove = 0.f;

private:
	CShader*						m_pShaderCom = nullptr;
	CShader*						m_pShader_Rect = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModel_Spear = nullptr;
	CVIBuffer_Rect*					m_pRectCom = nullptr;
	CTexture*						m_pTextureTrail = nullptr;
	CTexture*						m_pTextureTrailCenter = nullptr;
	CTexture*						m_pTextureSpear = nullptr;
	CTexture*						m_pTextureBlur = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPantheon_Q_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
