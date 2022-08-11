#pragma once

#include "RenderTarget.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);

public:
	HRESULT Add_RenderTarget(const _tchar* pTargetTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);


#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Ready_Debug_TargetDesc(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _tchar* pMRTTag);
#endif // _DEBUG

private:
	map<const _tchar*, class CRenderTarget*>			m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>	RENDERTARGETS;

private: /* 장치에 동시에 셋팅되어야할 렌더타겟들. */
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTS;



private:
	ID3D11RenderTargetView*				m_pOldRTV = nullptr;
	ID3D11DepthStencilView*				m_pOldDSV = nullptr;

#ifdef _DEBUG
private:
	class CVIBuffer_Rect*				m_pDebugBuffer = nullptr;
	class CShader*						m_pDebugShader = nullptr;

	_float4x4							m_ProjMatrix;
#endif // _DEBUG



private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);




public:
	virtual void Free() override;
};

END

