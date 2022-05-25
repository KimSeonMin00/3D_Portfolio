#include "..\Public\VIBuffer.h"


CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	:CComponent(rhs),
	m_pVB(rhs.m_pVB),
	m_VBDesc(rhs.m_VBDesc),
	m_VBSubResourceData(rhs.m_VBSubResourceData),
	m_iNumVertexBuffer(rhs.m_iNumVertexBuffer),
	m_iNumVertices(rhs.m_iNumVertices),
	m_iStride(rhs.m_iStride),

	m_pIB(rhs.m_pIB),
	m_IBDesc(rhs.m_IBDesc),
	m_IBSubResourceData(rhs.m_IBSubResourceData),
	m_eFormat(rhs.m_eFormat),
	m_eTopology(rhs.m_eTopology),
	m_iNumPrimitive(rhs.m_iNumPrimitive),
	m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive),
	m_iIndicesStride(rhs.m_iIndicesStride)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffer[] = {
		m_pVB
	};

	_uint			iStrides[]{
		m_iStride
	};

	_uint			iOffsets[]{
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffer, pVertexBuffer, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_pDeviceContext->DrawIndexed(m_iNumPrimitive*m_iNumIndicesPerPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Ready_Vertex_Buffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Ready_Index_Buffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
