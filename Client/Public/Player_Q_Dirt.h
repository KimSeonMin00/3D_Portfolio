#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CModel;
class CShader;
class CTexture;
END

class CPlayer_Q_Dirt
	:public CGameObject
{
public:
	typedef struct tagTranformAlpha
	{
		_uint	iTextureIndex = 0;
		_float	fSpeedX = 1.f;
		_float  fSpeedY = 1.f;
		_float  fRadian = 0.f;
		_float  fScale = 1.f;
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector vAxis = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		
	}TRANSFORMALPHA;

protected:
	explicit CPlayer_Q_Dirt(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_Q_Dirt(const CPlayer_Q_Dirt& rhs);
	virtual ~CPlayer_Q_Dirt() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT SetUp_ConstantTable_2(_uint iIndex);

	HRESULT Render_Crack();
	HRESULT Render_Rock();

private:
	vector<TRANSFORMALPHA*>		m_vecTA;
	vector<TRANSFORMALPHA*>		m_vecTARock;
	_float						m_fLiveTime = 0.f;
	_float						m_fCrack_Alpha = 1.f;

private:
	CRenderer*					m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CShader*					m_pModel_ShaderCom = nullptr;
	CVIBuffer_Rect*				m_pRectCom= nullptr;
	CModel*						m_pModelCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pTextureCrack_Com = nullptr;

public:
	static CPlayer_Q_Dirt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

