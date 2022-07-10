#pragma once

#include "Client_Defines.h"
#include "Monster.h"

class CVoli_Ghost
	final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_Q, STATE_STUN, STATE_END };

protected:
	explicit CVoli_Ghost(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CVoli_Ghost(const CVoli_Ghost& rhs);
	virtual ~CVoli_Ghost() = default;

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

	void	Move(_float fTimeDelta);
	void	Q_Skill(_float fTimeDelta);
	void	Stun(_float fTimeDelta);

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bIsChanneling = false;
	_bool			m_bStateChange = false;
	_bool			m_bIsState_In = false;
	_bool			m_bStop = false;

	_float			m_fTime = 0.f;

	_uint			m_iCurrentIndex = 0;

private://For Move
	_vector			m_vMovePos;
	_vector			m_vMoveDir;
	_float			m_fMoveDistTotal = 0.f;
	_float			m_fMoveDist = 0.f;

	_float			m_fMoveSpeed = 2.f;

public:
	static CVoli_Ghost* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

