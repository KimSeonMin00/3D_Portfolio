#pragma once
#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	const _float3* Get_Point(POINT ePoint) {
		return &m_vPoint[ePoint];
	}

	_uint Get_Index() {
		return m_iIndex;
	}

public:
	void SetUp_Neighbor(LINE eLine, _uint iIndex) {
		m_iNeighborIndex[eLine] = iIndex;
	}

public:
	HRESULT NativeConstruct(const _float3* pPoints, _uint iIndex);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isIn(const _float3* pPosition, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	_float3				m_vPoint[POINT_END];
	_float3				m_vLine[LINE_END];
	_int				m_iNeighborIndex[LINE_END] = { -1, -1, -1 };
	_uint				m_iIndex = 0;

#ifdef _DEBUG
private:
	class CVIBuffer_Triangle*		m_pVIBuffer = nullptr;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Ready_Debug_Buffer();

#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END

