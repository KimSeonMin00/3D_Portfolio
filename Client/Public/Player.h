#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CPlayer final 
	: public CGameObject
{
public:
	enum STATE {STATE_IDLE, STATE_MOVE, STATE_END};

public:
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

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void Change_AnimtionIndex(_uint iIndex);

private:
	void	Key_Input(_float fTimeDelta);
	void	SetUp_Animation_Index(_float fTimeDelta);

	void	Change_State();
	void	Update_State(_float fTimeDelta);

	void	Move(_float fTimeDelta);

private:
	STATE			m_ePreState = STATE_END;
	STATE			m_eState = STATE_END;
	_bool			m_bIsChange_State = false;

	_float			m_fKeyInputTimeAcc = 0;
	_float			m_fKeyInputTime = 0.2f;

	_uint m_iAnimationIndex = 0;

private://For Move
	_vector		m_vMovePos;
	_vector		m_vMoveDir;
	_float			m_fMoveDistTotal = 0.f;
	_float			m_fMoveDist = 0.f;

	_float			m_fMoveSpeed = 2.f;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END