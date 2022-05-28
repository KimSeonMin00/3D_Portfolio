#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract 
	:public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye;
		_float4		vAt;
		_float4		vAxisY;

		_float		fFovy, fAspect, fNear, fFar;
	}CAMERADESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	CAMERADESC					m_CameraDesc;
	class CPipeline*			m_pPipeline = nullptr;

protected:
	HRESULT Bind_TransformMatrices();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
