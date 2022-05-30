#include "stdafx.h"
#include "..\Public\ImguiManager.h"
#include "GameInstance.h"

#include "Transform.h"

const char* CImguiManager::CurrentItem = nullptr;

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
			m_iNumItems++;
	}

	ImGui::PushItemWidth(100);

	if (m_iNumItems > 0)
	{
		char Item[256] = "";
		sprintf_s(Item, "UI_%d", m_iCurrentItemIndex);

		CurrentItem = Item;
	}

	if (ImGui::BeginCombo("UI", CurrentItem))
	{
		for (int n = 0; n < m_iNumItems; n++)
		{
			char Item[256] = "";
			sprintf_s(Item, "UI_%d", n);

			bool is_selected = (CurrentItem == Item); // You can store your selection however you want, outside or inside your objects

			if (ImGui::Selectable(Item, is_selected))
			{
				CurrentItem = Item;
				m_iCurrentItemIndex = n;

				if (m_pTransform != nullptr)
					Safe_Release(m_pTransform);

				m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Com_Transform"), n);
				Safe_AddRef(m_pTransform);
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}

	_float3 fScale = {0.f, 0.f, 0.f};
	_float3 fPosition = { 0.f, 0.f, 0.f };

	if (m_pTransform != nullptr)
	{
		fScale = m_pTransform->Get_Scaled();
		

		ImGui::Text("Scale");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Scale.x", &fScale.x); ImGui::SameLine();
		ImGui::InputFloat("Scale.y", &fScale.y); ImGui::SameLine();
		ImGui::InputFloat("Scale.z", &fScale.z);

		m_pTransform->Set_Scaled(XMLoadFloat3(&fScale));

		XMStoreFloat3(&fPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

		fPosition.x = fPosition.x + (g_iWinCX >> 1);
		fPosition.y = -fPosition.y + (g_iWinCY >> 1);

		if (m_pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON))
		{
			POINT		ptMouse;

			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			_float3		vMousePos;

			vMousePos.x = ((_float)ptMouse.x);
			vMousePos.y = ((_float)ptMouse.y);
			vMousePos.z = 0.f;

			fPosition.x = vMousePos.x;
			fPosition.y = vMousePos.y;
			fPosition.z = vMousePos.z;
		}

		ImGui::Text("Position");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Position.x", &fPosition.x); ImGui::SameLine();
		ImGui::InputFloat("Position.y", &fPosition.y); ImGui::SameLine();
		ImGui::InputFloat("Position.z", &fPosition.z);

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x - (g_iWinCX >> 1), -fPosition.y + (g_iWinCY >> 1), 0.f, 1.f));
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