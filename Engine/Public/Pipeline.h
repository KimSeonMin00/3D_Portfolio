#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeline final:
	public CBase
{
	DECLARE_SINGLETON(CPipeline);

public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeline();
	virtual ~CPipeline() = default;

public:
	_float4 Get_CamPositionFloat4() const {
		return m_vCamPosition;
	}

	_vector Get_CamPositionVector() const {
		return XMLoadFloat4(&m_vCamPosition);
	}

	_matrix Get_TransformMatrix(TRANSFORMSTATETYPE eStateType) {
		return XMLoadFloat4x4(&m_TransformMatrix[eStateType]);
	}

	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType) {
		return m_TransformMatrix[eStateType];
	}

	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType) {
		return m_TransposeMatrix[eStateType];
	}
public:
	void Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix[eStateType], TransformMatrix);
	}

public:
	void Tick();

private:
	_float4x4			m_TransformMatrix[D3DTS_END];
	_float4x4			m_TransposeMatrix[D3DTS_END];
	_float4				m_vCamPosition;

public:
	virtual void Free() override;
};
END
