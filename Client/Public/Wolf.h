#pragma once
#include "Client_Defines.h"
#include "Monster.h"

class CWolf
	final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_AGGRO, STATE_MOVE, STATE_ATTACK, STATE_DEATH, STATE_END };


protected:
	explicit CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CWolf(const CWolf& rhs);
	virtual ~CWolf() = default;

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
	void	Change_State(_float fTimeDelta);
	void	Check_Loop(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);

	void	Idle(_float fTimeDelta);
	void	Idle_Aggro(_float fTimeDelta);
	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	Death(_float fTimeDelta);
	
	_float	m_fInitTime = 0.f;
	_float	m_fStunTime = 0.f;
	_bool	m_bStop = false;
	_bool	m_bStun = false;

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bIsChanneling = false;
	_bool			m_bStateChange = false;
	_bool			m_bState_In = false;

	_uint			m_iCurrentIndex = 0;
	_uint			m_iAttackIndex = 4;

public:
	static CWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

