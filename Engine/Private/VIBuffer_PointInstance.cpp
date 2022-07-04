#include "..\Public\VIBuffer_PointInstance.h"


CVIBuffer_PointInstance::CVIBuffer_PointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(const CVIBuffer_PointInstance & rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubResourceData(rhs.m_VBInstSubResourceData)
	, m_iInstStride(rhs.m_iInstStride)
	, m_pSpeed(rhs.m_pSpeed)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_PointInstance::NativeConstruct_Prototype(_uint iNumInstance)
{
#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumVertexBuffer = 2;
	m_iNumInstance = iNumInstance;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXPOINT*			pVertices = new VTXPOINT;

	pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	pVertices[0].fPSize = 1.f;

	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicesPerPrimitive = 1;
	m_iIndicesStride = sizeof(_ushort);
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iNumPrimitive * m_iIndicesStride;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion


#pragma region INSTANCE_BUFFER

	m_iInstStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBInstDesc.ByteWidth = m_iInstStride * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.StructureByteStride = m_iInstStride;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVtx = new VTXMATRIX[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstanceVtx[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVtx[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVtx[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVtx[i].vTranslation = _float4(rand() % 10, 10.f, rand() % 10, 1.f);
	}

	m_VBInstSubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);

#pragma endregion

	m_pSpeed = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeed[i] = rand() % 10 + 10;
	}
	return S_OK;
}

HRESULT CVIBuffer_PointInstance::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_PointInstance::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffer[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstStride
	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffer, pVertexBuffer, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->DrawIndexedInstanced(1, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_PointInstance::Update(_double TimeDelta)
{
	if (nullptr == m_pVBInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vTranslation.y < 0.f)
			((VTXMATRIX*)SubResource.pData)[i].vTranslation.y = 10.f;

	}



	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_PointInstance * CVIBuffer_PointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumInstance)
{
	CVIBuffer_PointInstance*		pInstance = new CVIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		MSGBOX(TEXT("Failed to Created : CVIBuffer_RectInstance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_PointInstance::Clone(void * pArg)
{
	CVIBuffer_PointInstance*		pInstance = new CVIBuffer_PointInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CVIBuffer_RectInstance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_PointInstance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeed);

	Safe_Release(m_pVBInstance);
}
