#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final:
	public CComponent
{
public:
	typedef struct tagPassDesc
	{
		ID3DX11EffectPass*			pPass = nullptr;
		ID3D11InputLayout*			pInputLayout = nullptr;
	}PASSDESC;

private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Begin(_uint iPassIndex = 0);
	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT Set_SRV(const char* pConstantName, ID3D11ShaderResourceView* pSRV);

private:
	ID3DX11Effect*				m_pEffect = nullptr;

	_uint							m_iNumPasses = 0;
	vector<PASSDESC>		m_PassDesces;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END

