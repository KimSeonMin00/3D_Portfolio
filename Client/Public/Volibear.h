#pragma once
#include "Client_Defines.h"
#include "Monster.h"

class CVolibear
	final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_Q, STATE_W, STATE_W_BITE, STATE_E, STATE_R, STATE_FLY, STATE_STUN, STATE_DEATH, STATE_END };


protected:
	explicit CVolibear(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVolibear(const CVolibear& rhs);
	virtual ~CVolibear() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


private:
	void			Update_HandCollider();
private:
	CTransform*						m_pRightSparkTransform = nullptr;
	CTransform*						m_pLeftSparkTransform = nullptr;

	CCollider*						m_pSPHEREAttackRange = nullptr;
	CCollider*						m_pOBBRightHand = nullptr;
	CCollider*						m_pOBBLeftHand = nullptr;
	CCollider*						m_pOBBJaw = nullptr;

	_float4x4		m_PivotMatrix;

	CHierarchyNode*	m_pRHNode = nullptr;
	CHierarchyNode*	m_pLHNode = nullptr;
	CHierarchyNode*	m_pJawNode = nullptr;

	CGameObject* m_pHP = nullptr;

private:
	void	Change_State(_float fTimeDelta);
	void	Check_Loop(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);

	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	Q_Skill(_float fTimeDelta);
	void	W_Skill(_float fTimeDelta);
	void	W_Bite_Skill(_float fTimeDelta);
	void	E_Skill(_float fTimeDelta);
	void	R_Skill(_float fTimeDelta);
	void	Death(_float fTimeDelta);
	void	Fly(_float fTimeDelta);
	void	Stun(_float fTimeDelta);
	void	Grab(_float fTimeDelta);

	void	Pattern_1(_float fTimeDelta);
	void	Pattern_2(_float fTimeDelta);
	void	Pattern_3(_float fTimeDelta);
	void	Pattern_4(_float fTimeDelta);
	void	Pattern_5(_float fTimeDelta);
	void	Pattern_6(_float fTimeDelta);

	void	Pattern_Phase1(_float fTimeDelta);
	_uint	m_iPatternIndex = 0;
	_bool	m_bPatternFinished = false;

	_float	m_fInitTime = 0.f;
	_float  m_fPatternTime = 0.f;
	_float	m_fStunTime = 0.f;
	_bool	m_bStop = false;
	_bool	m_bStun = false;
	_bool	m_bGrab = false;
	_bool	m_bCutScene = false;

	_float	m_fFlyAttackDelay = 0.f;


private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bIsChanneling = false;
	_bool			m_bStateChange = false;
	_bool			m_bIsState_In = false;

	_bool			m_bSkillFinished = false;

	_uint			m_iCurrentIndex = 0;

private://For Attack
	_uint			m_iAttackIndex = 3;

private://For Q
	_bool			m_bQState = false;
	_bool			m_bQState_Pre = false;
	_bool			m_bQAttack = false;

	_float			m_fQTime = 0.f;
	_float          m_fFootprintTime = 0.5f;

private://For R
	_bool			m_bRState = false;

	_float			m_fRTime = 0.f;

	_float			m_fTime = 0.f;

	_float3			m_vOriginScale;

	_float  m_fDelayTime = 0.f;
	_bool	 m_bPattern1 = false;

private://For Pattern 2
	_uint		m_iPattern_AttackTime = 0;
	_int		m_iPattern_Index = 0;

public:
	static CVolibear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

