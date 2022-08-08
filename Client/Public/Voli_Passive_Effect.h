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
class CVoli_Passive_Effect
	:public CGameObject
{
public:
	typedef struct tagSparkData
	{
		_uint  iIndex = 0;
		_float fScale = 0.5f;
		_float fAlpha = 1.f;
		_float fRadian = 0.f;

	}SPARKDATA;

protected:
	explicit CVoli_Passive_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVoli_Passive_Effect(const CVoli_Passive_Effect& rhs);
	virtual ~CVoli_Passive_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	vector<SPARKDATA*>			m_vecSpark;
	_float						m_fAddTime = 0.f;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pRect_Buffer = nullptr;
	CTexture*					m_pTextureSpark = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static CVoli_Passive_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
