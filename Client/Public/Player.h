#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CHierarchyNode;
class CNavigation;
END

BEGIN(Client)

class CPlayer final 
	: public CGameObject
{
public:
	enum STATE {STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_Q, STATE_W, STATE_E, STATE_R, STATE_END};

protected:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pAABBCom = nullptr;
	CCollider*						m_pOBBCom = nullptr;
	CCollider*						m_pTestCom = nullptr;
	CCollider*						m_pSPHERECom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void Change_AnimtionIndex(_uint iIndex);

private:
	void	Key_Input(_float fTimeDelta);

	void	Change_State(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);
	void	Update_SwordCollider();

	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	Q_Skill(_float fTimeDelta);
	void	W_Skill(_float fTimeDelta);
	void	E_Skill(_float fTimeDelta);
	void	R_Skill(_float fTimeDelta);

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	STATE			m_eDoingState = STATE_END;

	_bool			m_bWeapon_Out = false;
	_bool			m_bIsChanneling = false;
	_bool			m_bIsMotionChange = false;
	_bool			m_bStateChange = false;

	_float			m_fKeyInputTimeAcc = 0;
	_float			m_fKeyInputTime = 0.2f;

	_uint m_iAnimationIndex = 0;

private://For Attack Collider
	_float4x4		m_PivotMatrix;

	CHierarchyNode*	m_pSwordNode = nullptr;

private:
	_bool		m_bSeathing = false;

private://For Move
	_vector		m_vMovePos;
	_vector		m_vMoveDir;
	_float			m_fMoveDistTotal = 0.f;
	_float			m_fMoveDist = 0.f;

	_float			m_fMoveSpeed = 4.f;
	_bool			m_bMove_Stop = false;

private://For Attack
	_uint			m_iAttackAnimation_Index = 0;

private:
	_uint			m_iQAnimation_Index = 27;
	_uint			m_iQ_Time = 0;

	_float			m_fQDistance = 3.f;

private:
	_uint			m_iEAnimation_Index = 35;
	_bool			m_bE_Q_Used = false;

	_float			m_fDashDist = 0.f;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END