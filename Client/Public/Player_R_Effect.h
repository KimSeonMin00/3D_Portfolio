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
class CPlayer_R_Effect final
	:public CGameObject
{
public:
	typedef struct tagTranformAlpha
	{
		_float2		vTextureMove = _float2(0.f, -0.5f);
		_float		fAlpha = 1.f;
		_vector		fScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		_vector		vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}TRANSFORMALPHA;

protected:
	explicit CPlayer_R_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_R_Effect(const  CPlayer_R_Effect& rhs);
	virtual ~CPlayer_R_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Render_Flash();
	HRESULT Render_Wind();
	HRESULT Render_String();
	HRESULT Render_Crack();

private:
	_float						m_fAddTime = 0.2f;
	_float						m_fTexMoveX = 0.f;
	_float						m_fAlpha = 1.f;
	vector<TRANSFORMALPHA*>		m_vecTransformAlpha;

	vector<TRANSFORMALPHA*>		m_vecString;
	_bool						m_bStringFall = false;

	vector<TRANSFORMALPHA*>		m_vecRing;

private:
	_float						m_fFlashScale = 0.1f;
	_float						m_fFlashAlpha = 1.f;

	_float						m_fCrackAlpha = 1.f;



private:
	CShader*					m_pShaderCom = nullptr;
	CShader*					m_pShaderCom_Rect = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Spark = nullptr;
	CVIBuffer_Rect*				m_pRect_String = nullptr;
	CVIBuffer_Rect*				m_pRect_Crack = nullptr;
	CTexture*					m_pTexture = nullptr;
	CTexture*					m_pTextureSpark = nullptr;
	CTexture*					m_pTextureString = nullptr;
	CTexture*					m_pTexture_R_Ring = nullptr;
	CTexture*					m_pTexture_Crack = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iNumModel);

public:
	static CPlayer_R_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END

