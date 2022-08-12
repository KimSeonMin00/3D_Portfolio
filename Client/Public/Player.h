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
	virtual HRESULT Render_ShadowDepth();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pAABBCom = nullptr;
	CCollider*						m_pHitSphereCom = nullptr;
	CCollider*						m_pOBBCom = nullptr;
	CCollider*						m_pSPHERECom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void	Grabbed(_vector pPos)
	{
		m_bGrab = true;

		pPos = XMVectorSetY(pPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		m_pTransformCom->LookAt(pPos);
	};
	void	Set_Fall()
	{
		m_bGrab = true;
		m_bFall = true;
		m_fYdir = 3.f;
	};

	void Damaged(_float fDamage);

	void Knock_Back(_vector pPos)
	{
		m_bKnockBack = true;

		pPos = XMVectorSetY(pPos, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		m_pTransformCom->LookAt(pPos);
		m_fKnockBackTime = 0.f;

		m_eState = STATE_IDLE;
	}

private:
	void	Key_Input(_float fTimeDelta);
	_bool	Cast_R(_float fTimeDelta);

	void	Change_State(_float fTimeDelta);
	void	Update_State(_float fTimeDelta);
	void	Update_SwordCollider();

	void	Move(_float fTimeDelta);
	void	Attack(_float fTimeDelta);
	void	Q_Skill(_float fTimeDelta);
	void	W_Skill(_float fTimeDelta);
	void	E_Skill(_float fTimeDelta);
	void	R_Skill(_float fTimeDelta);

	void	Fall(_float fTimeDelta);

private:
	void	Hit_Monster(_uint iIndex);
	void	Initialize_Hit();

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
	_matrix		m_SwordWorldMat;

	CHierarchyNode*	m_pSwordNode = nullptr;

private:
	_bool		m_bSeathing = false;

private:
	_float		m_fHealthPoint = 2000.f;
	_float		m_fMaxHealth = 2000.f;

	CGameObject* m_pHP = nullptr;

private://For Move
	_vector		m_vMovePos;
	_vector		m_vMoveDir;
	_float			m_fMoveDistTotal = 0.f;
	_float			m_fMoveDist = 0.f;

	_float			m_fMoveSpeed = 4.f;
	_bool			m_bMove_Stop = false;

private://For Attack
	_uint			m_iAttackAnimation_Index = 0;
	_bool			m_bHit = false;

private:
	_uint			m_iQAnimation_Index = 27;
	_uint			m_iQ_Time = 0;

	_float			m_fQDistance = 3.f;

private:
	_uint			m_iEAnimation_Index = 35;
	_bool			m_bE_Q_Used = false;

	_float			m_fDashDist = 0.f;
	_float			m_fAfterImageTime = 0.2f;

	//For R
private:
	_float			m_fRAttackTime = 0.f;
	_uint			m_iAttackCount = 0;

private:
	_float			m_fYdir = 1.f;


	_bool			m_bFall = false;
	_bool			m_bGrab = false;
	_bool			m_bKnockBack = false;
	_float          m_fKnockBackTime = 0.f;

private:
	list<_uint*>			m_MonsterIndexList;

	_float			m_fDamage = 100.f;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END