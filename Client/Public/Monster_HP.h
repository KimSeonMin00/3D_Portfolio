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
class CMonster_HP
	:public CGameObject
{
protected:
	explicit CMonster_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CMonster_HP(const CMonster_HP& rhs);
	virtual ~CMonster_HP() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_Ratio(_float fRatio)
	{
		m_fRatio = fRatio;
	}

	void		Set_Pos(_vector vPos)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	_float								m_fRatio = 1.f;
	_float								m_fScale = 0.6f;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTexture*						m_pTexture_Frame = nullptr;
	CTexture*						m_pTexture_HP = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

public:
	static CMonster_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
