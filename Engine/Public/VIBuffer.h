#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract :
	public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	const _float3* Get_VerticesPos() {
		return m_pVerticesPos;
	}

	void* Get_Indices() {
		return m_pIndices;
	}

	_uint Get_NumPrimitive() {
		return m_iNumPrimitive;
	}

	DXGI_FORMAT	 Get_IndexFormat() {
		return m_eFormat;
	}

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	virtual HRESULT Render();

protected://Vertex Buffer 변수
	ID3D11Buffer*				m_pVB = nullptr;
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubResourceData;

	_uint						m_iNumVertexBuffer = 0;
	_uint						m_iNumVertices = 0;
	_uint						m_iStride = 0;

	_float3*					m_pVerticesPos = nullptr;

protected://Index Buffer 변수
	ID3D11Buffer*				m_pIB = nullptr;
	D3D11_BUFFER_DESC			m_IBDesc;
	D3D11_SUBRESOURCE_DATA		m_IBSubResourceData;
	DXGI_FORMAT					m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY    m_eTopology;

	_uint						m_iNumPrimitive = 0;
	_uint						m_iNumIndicesPerPrimitive = 0;
	_uint						m_iIndicesStride = 0;

	void*						m_pIndices = nullptr;
protected:
	HRESULT Ready_Vertex_Buffer();
	HRESULT Ready_Index_Buffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

