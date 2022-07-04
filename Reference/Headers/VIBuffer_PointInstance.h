#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer
{

protected:
	CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render() override;

public:
	void Update(_double TimeDelta);

private:
	_uint						m_iNumInstance = 0;
	ID3D11Buffer*				m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC			m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA		m_VBInstSubResourceData;
	_uint						m_iInstStride;

	_float*						m_pSpeed = nullptr;



public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
