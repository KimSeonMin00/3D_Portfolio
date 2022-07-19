#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
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
	_float						m_fAddTime = 0.2f;
	_float						m_fTexMoveX = 0.f;
	_float						m_fAlpha = 1.f;
	vector<TRANSFORMALPHA*>		m_vecTransformAlpha;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CTexture*					m_pTexture = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_uint iNumModel);

public:
	static CPlayer_R_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END

