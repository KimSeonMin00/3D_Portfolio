#include "stdafx.h"
#include "..\Public\ImguiManager.h"
#include "GameInstance.h"

CImguiManager::CImguiManager()
	:m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImguiManager::NativeConstruct(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
{
	if (pDevice == nullptr || pDevice_Context == nullptr)
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pDevice_Context);

	return S_OK;
}

void CImguiManager::Tick(_double TimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Test");
	if(ImGui::Button("Create"))
	{
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"));
	}
	ImGui::End();
}

HRESULT CImguiManager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

CImguiManager * CImguiManager::Create(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
{
	CImguiManager* pInstance = new CImguiManager();

	if (FAILED(pInstance->NativeConstruct(hWnd, pDevice, pDevice_Context)))
		return nullptr;

	return pInstance;
}

void CImguiManager::Free()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pGameInstance);
}