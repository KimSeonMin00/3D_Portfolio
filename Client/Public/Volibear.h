#pragma once
#include "Client_Defines.h"
#include "Monster.h"

class CVolibear
	final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_Q, STATE_W, STATE_E, STATE_R, STATE_END };


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
	CCollider*						m_pSPHEREAttackRange = nullptr;
	CCollider*						m_pOBBRightHand = nullptr;
	CCollider*						m_pOBBLeftHand = nullptr;

	_float4x4		m_PivotMatrix;

	CHierarchyNode*	m_pRHNode = nullptr;
	CHierarchyNode*	m_pLHNode = nullptr;

private:
	void	Key_Input(_float fTimeDelta);

	void	Change_State(_float fTimeDelta);
	void	Check_Loop(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);

	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	W_Skill(_float fTimeDelta);
	void	E_Skill(_float fTimeDelta);
	void	R_Skill(_float fTimeDelta);

	void	Chase_Player(_float fTimeDelta);

	_float	m_fInitTime = 0.f;

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bIsChanneling = false;
	_bool			m_bStateChange = false;
	_bool			m_bIsState_In = false;

	_uint			m_iCurrentIndex = 0;

private://For Move
	_vector			m_vMovePos;
	_vector			m_vMoveDir;
	_float			m_fMoveDistTotal = 0.f;
	_float			m_fMoveDist = 0.f;

	_float			m_fMoveSpeed = 2.f;

private://For Attack
	_uint			m_iAttackIndex = 3;

private://For Q
	_bool			m_bQState = false;
	_bool			m_bQState_Pre = false;
	_bool			m_bQAttack = false;

	_float			m_fQTime = 0.f;

private:
	_bool			m_bRState = false;

	_float			m_fRTime = 0.f;

	_float			m_fTime = 0.f;

	_float3			m_vOriginScale;
public:
	static CVolibear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

