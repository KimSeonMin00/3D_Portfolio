#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CPantheon_W_Effect
	:public CGameObject
{
public:
	typedef struct tagFireData
	{
		_uint  iIndex = 0;
		_float m_fScale = 2.f;
		_vector m_vDir;
		_vector m_vPos;
	}FIREDATA;


protected:
	explicit  CPantheon_W_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit  CPantheon_W_Effect(const   CPantheon_W_Effect& rhs);
	virtual ~CPantheon_W_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Slam();
	HRESULT Render_Crack();
	HRESULT Render_Fire();

private:
	_bool	m_bPlayer_Hit = false;

	_float  m_fLiveTime = 0.f;
	_float  m_fSlamX = 2.f;

	_float  m_fCrackAlpha = 1.f;
	_vector m_SourColor;
	_vector m_DestColor;

	vector<FIREDATA*> m_vecFire;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Com = nullptr;
	CTexture*					m_pTexture_Slam = nullptr;
	CTexture*					m_pTexture_Crack = nullptr;
	CTexture*					m_pTexture_Fire = nullptr;
	CCollider*					m_pSphere = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static  CPantheon_W_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
