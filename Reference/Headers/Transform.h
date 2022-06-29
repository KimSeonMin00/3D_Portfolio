#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final 
	:public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		tagTransformDesc() = default;
		tagTransformDesc(_float _fSpeedPerSec, _float _fRotationPerSec)
			: fSpeedPerSec(_fSpeedPerSec), fRotationPerSec(_fRotationPerSec) { }

		_float		fSpeedPerSec = 0.f;
		_float		fRotationPerSec = 0.f;
	}TRANSFORMDESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float3 Get_Scaled() const;

	void Set_State(STATE eState, _fvector vState) {
		/*_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		WorldMatrix.r[eState] = vState;
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);*/

		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
	}

	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc);
	void Set_Scaled(_fvector vScale);

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Go_Straight(_double TimeDelta, class CNavigation* pNaviCom = nullptr);
	HRESULT Go_Backward(_double TimeDelta);
	HRESULT Go_Right(_double TimeDelta);
	HRESULT Go_Left(_double TimeDelta);
	HRESULT Go_Direction(_fvector vDirection, _double TimeDelta);

	HRESULT Turn(_fvector vAxis, _double TimeDelta);
	HRESULT LookAt(_fvector vAt);

public:
	HRESULT Bind_OnShader(class CShader* pShader, const char* pConstantName);


private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;



public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const TRANSFORMDESC& TransformDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END