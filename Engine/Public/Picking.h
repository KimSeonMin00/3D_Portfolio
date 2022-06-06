#pragma once
#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final 
	: public CBase
{
	DECLARE_SINGLETON(CPicking)
private:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY);
	void Transform_ToWorldSpace();
	bool Picking(class CVIBuffer* pVIBuffer, class CTransform* pTransform, _float4* pOut);


private:
	HWND			m_hWnd;

	_uint				m_iWinCX;
	_uint				m_iWinCY;

private:
	_vector				m_vWorldRayDir, m_vWorldRayPos;
	class CPipeline*			m_pPipeline = nullptr;

public:
	virtual void Free() override;
};

END
