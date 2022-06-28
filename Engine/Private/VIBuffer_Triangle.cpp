#include "..\Public\VIBuffer_Triangle.h"

CVIBuffer_Triangle::CVIBuffer_Triangle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Triangle::NativeConstruct_Prototype(const _float3* pPoints)
{
#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = 3;
	m_iNumVertexBuffer = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXCOL*			pVertices = new VTXCOL[3];

	pVertices[0].vPosition = pPoints[0];
	pVertices[0].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[1].vPosition = pPoints[1];
	pVertices[1].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[2].vPosition = pPoints[2];
	pVertices[2].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER	

	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(_ushort) * 4;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	_ushort*		pIndices = new _ushort[4];
	ZeroMemory(pIndices, sizeof(_ushort) * 4);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);



#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Triangle::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Triangle::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffer[] = {
		m_pVB
	};

	_uint					iStrides[] = {
		m_iStride
	};

	_uint					iOffsets[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffer, pVertexBuffer, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_pDeviceContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

CVIBuffer_Triangle * CVIBuffer_Triangle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3* pPoints)
{
	CVIBuffer_Triangle*		pInstance = new CVIBuffer_Triangle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pPoints)))
	{
		MSGBOX(TEXT("Failed to Created : CVIBuffer_Triangle"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Triangle::Clone(void * pArg)
{
	CVIBuffer_Triangle*		pInstance = new CVIBuffer_Triangle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVIBuffer_Triangle"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free();
}
