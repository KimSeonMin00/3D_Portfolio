#include "stdafx.h"
#include "ImguiManager.h"

#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CGraphic_Device::GRAPHICDEVDESC		GraphicDevDesc;
	ZeroMemory(&GraphicDevDesc, sizeof(CGraphic_Device::GRAPHICDEVDESC));

	GraphicDevDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDevDesc.hWnd = g_hWnd;
	GraphicDevDesc.iWinCX = g_iWinCX;
	GraphicDevDesc.iWinCY = g_iWinCY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDevDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	m_pImgui_Manager = CImguiManager::Create(g_hWnd, m_pDevice, m_pDeviceContext);

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	m_pGameInstance->Tick_Engine(TimeDelta);

	if (nullptr != m_pImgui_Manager)
		m_pImgui_Manager->Tick(TimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	if (FAILED(m_pRenderer->Draw_Renderer()))
		return E_FAIL;

	m_pGameInstance->Render_Engine();

	if (nullptr != m_pImgui_Manager)
		m_pImgui_Manager->Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevel));	
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
}

HRESULT CMainApp::Ready_Gara()
{
	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	ZeroMemory(vPoints, sizeof(_float3) * 3);

	vPoints[0] = _float3(0.f, 0.2f, 5.f);
	vPoints[1] = _float3(5.f, 0.2f, 0.f);
	vPoints[2] = _float3(0.f, 0.2f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.2f, 5.f);
	vPoints[1] = _float3(5.f, 0.2f, 5.f);
	vPoints[2] = _float3(5.f, 0.2f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.2f, 10.f);
	vPoints[1] = _float3(5.f, 0.2f, 5.f);
	vPoints[2] = _float3(0.f, 0.2f, 5.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(5.f, 0.2f, 5.f);
	vPoints[1] = _float3(10.f, 0.2f, 0.f);
	vPoints[2] = _float3(5.f, 0.2f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pImgui_Manager);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}



