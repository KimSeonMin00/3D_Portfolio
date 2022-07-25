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
	_bool		Get_Airborne()
	{
		return m_bAirborne;
	};

	void		Damaged(_float fDamage)
	{
		if (m_bHit == false)
		{
			m_fHealthPoint -= fDamage;
			m_bHit = true;
			m_fHitDelay = 0.f;
		}
	}

	void		Set_Select()
	{
		m_bSelected = true;
	}

	_uint		Get_Index()
	{
		return m_iMonsterIndex;
	}

protected:
	void	Chase_Player(_float fTimeDelta);
	void	None_Overlap(_float fTimeDelta);

protected://For Move
	_vector			m_vMovePos;
	_vector			m_vMoveDir;
	_float				m_fMoveDistTotal = 0.f;
	_float				m_fMoveDist = 0.f;

	_float				m_fMoveSpeed = 2.f;

protected:
	_float		m_fMaxHealth = 1000.f;
	_float		m_fHealthPoint = 1000.f;

	_float		m_fHitDelay = 0.f;
	_bool		m_bHit = false;

	_bool		m_bSelected = false;
protected:
	_bool		m_bAirborne = false;
	_float		m_fAirborneTime = 0.f;

protected:
	_uint		m_iMonsterIndex = 0;

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