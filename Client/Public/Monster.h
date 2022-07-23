#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)

class CMonster
	:public CGameObject
{
protected:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void		Airborne(_float fTimeDelta);
	void		Set_Airborne()
	{
		if (m_bAirborne == false)
		{
			m_bAirborne = true;
			m_fAirborneTime = 0.f;
		}
	};

	void		Damaged(_float fDamage)
	{
		m_fHealthPoint -= fDamage;
	}

protected:
	void	Chase_Player(_float fTimeDelta);

protected://For Move
	_vector			m_vMovePos;
	_vector			m_vMoveDir;
	_float				m_fMoveDistTotal = 0.f;
	_float				m_fMoveDist = 0.f;

	_float				m_fMoveSpeed = 2.f;

protected:
	_float		m_fMaxHealth = 1000.f;
	_float		m_fHealthPoint = 1000.f;

protected:
	_bool		m_bAirborne = false;
	_float		m_fAirborneTime = 0.f;

protected:
	CCollider*						m_pAABBCom = nullptr;
	CCollider*						m_pSphereCom = nullptr;
	CShader*							m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END