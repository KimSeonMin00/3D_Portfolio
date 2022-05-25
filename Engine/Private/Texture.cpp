#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_ShaderResourceViews(rhs.m_ShaderResourceViews)
{
	for (auto& pSRV : m_ShaderResourceViews)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::NativeConstruct_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	_tchar			szFullPath[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		ID3D11ShaderResourceView*			pSRV = nullptr;

		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar		szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT hr = 0;

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_ShaderResourceViews.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_OnShader(CShader * pShader, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShader ||
		iTextureIndex >= m_ShaderResourceViews.size())
		return E_FAIL;

	return pShader->Set_SRV(pConstantName, m_ShaderResourceViews[iTextureIndex]);
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSGBOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_ShaderResourceViews)
		Safe_Release(pSRV);

	m_ShaderResourceViews.clear();
}

