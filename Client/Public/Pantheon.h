#pragma once

#include "Client_Defines.h"
#include "Monster.h"

class CPantheon
	final : public CMonster
{
public:
	enum STATE { STATE_INIT, STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_Q, STATE_W, STATE_E, STATE_R, STATE_DEATH, STATE_END };

protected:
	explicit CPantheon(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon(const CPantheon& rhs);
	virtual ~CPantheon() = default;

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
	_float4x4		m_PivotMatrix;

private:
	void	Key_Input(_float fTimeDelta);
	void	Change_State(_float fTimeDelta);
	void	Check_Loop(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);

	void	Init(_float fTimeDelta);
	void	Idle(_float fTimeDelta);
	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	Q_Skill(_float fTimeDelta);
	void	W_Skill(_float fTimeDelta);
	void	E_Skill(_float fTimeDelta);
	void	R_Skill(_float fTimeDelta);
	void	Death(_float fTimeDelta);

	void	Pattern_1(_float fTimeDelta);

	_uint	m_iPattern_AttackTime = 0;
	_float	m_fInitTime = 0.f;
	_bool	m_bStop = false;

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bIsChanneling = false;
	_bool			m_bStateChange = false;
	_bool			m_bIdle_In = false;

	_uint			m_iCurrentIndex = 0;

private://For Attack
	_bool			m_bAttackIndex_Change = false;
	_bool			m_bW_3Attack = false;
	_uint			m_iAttackIndex = 0;

private:
	_float          m_f_E_SlashTime = 0.2f;

private:
	_float			m_fE_CastingTime = 0.f;

public:
	static CPantheon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

