#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Triangle final : public CVIBuffer
{

protected:
	CVIBuffer_Triangle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Triangle(const CVIBuffer_Triangle& rhs);
	virtual ~CVIBuffer_Triangle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _float3* pPoints);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render();

public:
	static CVIBuffer_Triangle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
