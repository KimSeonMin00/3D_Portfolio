#include "stdafx.h"
#include "..\Public\ImguiManager.h"
#include "GameInstance.h"

#include "Transform.h"
#include "UI.h"
#include "Terrain.h"

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

	if(ImGui::Begin("Tool_List"))
	{
		if(ImGui::Button("UI"))
		{
			m_eToolList = TOOL_UI;			
		}

		if (ImGui::Button("Object"))
		{
			m_eToolList = TOOL_OBJECT;
		}

		switch (m_eToolList)
		{
		case TOOL_UI:
			ImGui::Begin("UI_Tool");
			UI_Tool();
			ImGui::End();
			break;

		case TOOL_OBJECT:
			ImGui::Begin("Object_Tool");
			Object_Tool();
			ImGui::End();
			break;

		default:
			break;
		}

		ImGui::End();
	}
}

HRESULT CImguiManager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImguiManager::UI_Tool()
{
	if (ImGui::Button("Create"))
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
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}

	if (nullptr != CurrentItem)
	{
		CUI* pSelectedUI = (CUI*)m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_UI"), m_iCurrentItemIndex);

		if (nullptr != pSelectedUI)
		{
			Safe_AddRef(pSelectedUI);
			pSelectedUI->Be_Selected();
			Safe_Release(pSelectedUI);
		}

		m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Com_Transform"), m_iCurrentItemIndex);
		if (nullptr == m_pTransform)
			return;

		Safe_AddRef(m_pTransform);

		_float3 fScale = { 0.f, 0.f, 0.f };
		_float3 fPosition = { 0.f, 0.f, 0.f };


		fScale = m_pTransform->Get_Scaled();


		ImGui::Text("Scale");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Scale.x", &fScale.x); ImGui::SameLine();
		ImGui::InputFloat("Scale.y", &fScale.y); ImGui::SameLine();

		m_pTransform->Set_Scaled(XMLoadFloat3(&fScale));

		XMStoreFloat3(&fPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

		fPosition.x = fPosition.x + (g_iWinCX >> 1);
		fPosition.y = -fPosition.y + (g_iWinCY >> 1);
		fPosition.z = 1.f;

		if (m_pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON))
		{
			POINT		ptMouse;

			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			_float3		vMousePos;

			vMousePos.x = ((_float)ptMouse.x);
			vMousePos.y = ((_float)ptMouse.y);
			vMousePos.z = 1.f;

			fPosition.x = vMousePos.x;
			fPosition.y = vMousePos.y;
			fPosition.z = 0.f;
		}

		ImGui::Text("Position");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Position.x", &fPosition.x); ImGui::SameLine();
		ImGui::InputFloat("Position.y", &fPosition.y);

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x - (g_iWinCX >> 1), -fPosition.y + (g_iWinCY >> 1), 0.f, 1.f));

		Safe_Release(m_pTransform);
	}

	if (ImGui::Button("Save"))
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileSaveDialog *pFileSave;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
				IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

			if (SUCCEEDED(hr))
			{

				DWORD dwFlags;

				hr = pFileSave->GetOptions(&dwFlags);

				if (SUCCEEDED(hr))
				{
					COMDLG_FILTERSPEC rgSpec[] =
					{
						{ L"DataFiles(*.dat)", L"*.dat" }
					};

					hr = pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

					if (SUCCEEDED(hr))
					{
						hr = pFileSave->SetDefaultExtension(L"dat");

						if (SUCCEEDED(hr))
						{
							// Show the Open dialog box.
							hr = pFileSave->Show(NULL);

							// Get the file name from the dialog box.
							if (SUCCEEDED(hr))
							{
								IShellItem *pItem;
								hr = pFileSave->GetResult(&pItem);
								if (SUCCEEDED(hr))
								{
									PWSTR pszFilePath;
									hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									// Display the file name to the user.
									if (SUCCEEDED(hr))
									{
										HANDLE		hFile = CreateFile(pszFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

										if (INVALID_HANDLE_VALUE == hFile)
											return;

										DWORD	dwByte = 0;

										for (_uint i = 0; i < m_iNumItems; i++)
										{
											m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Com_Transform"), i);
											if (nullptr == m_pTransform)
												return;

											Safe_AddRef(m_pTransform);

											_float3 fScale = { 0.f, 0.f, 0.f };
											_float3 fPosition = { 0.f, 0.f, 0.f };


											fScale = m_pTransform->Get_Scaled();
											XMStoreFloat3(&fPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

											Safe_Release(m_pTransform);

											WriteFile(hFile, &fScale.x, sizeof(_float), &dwByte, nullptr);
											WriteFile(hFile, &fScale.y, sizeof(_float), &dwByte, nullptr);

											WriteFile(hFile, &fPosition.x, sizeof(_float), &dwByte, nullptr);
											WriteFile(hFile, &fPosition.y, sizeof(_float), &dwByte, nullptr);

										}

										CloseHandle(hFile);
									}
									pItem->Release();
								}
							}
						}
					}
				}
				pFileSave->Release();
			}
			CoUninitialize();
		}
	}
	if (ImGui::Button("Load"))
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileSaveDialog *pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{

				DWORD dwFlags;

				hr = pFileOpen->GetOptions(&dwFlags);

				if (SUCCEEDED(hr))
				{
					COMDLG_FILTERSPEC rgSpec[] =
					{
						{ L"DataFiles(*.dat)", L"*.dat" }
					};

					hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

					if (SUCCEEDED(hr))
					{
						hr = pFileOpen->SetDefaultExtension(L"dat");

						if (SUCCEEDED(hr))
						{
							// Show the Open dialog box.
							hr = pFileOpen->Show(NULL);

							// Get the file name from the dialog box.
							if (SUCCEEDED(hr))
							{
								IShellItem *pItem;
								hr = pFileOpen->GetResult(&pItem);
								if (SUCCEEDED(hr))
								{
									PWSTR pszFilePath;
									hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									// Display the file name to the user.
									if (SUCCEEDED(hr))
									{
										HANDLE		hFile = CreateFile(pszFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

										m_iNumItems = 0;

										if (INVALID_HANDLE_VALUE == hFile)
											return;

										DWORD	dwByte = 0;

										while (true)
										{
											_float3 fScale = { 0.f, 0.f, 0.f };
											_float3 fPosition = { 0.f, 0.f, 0.f };

											ReadFile(hFile, &fScale.x, sizeof(_float), &dwByte, nullptr);
											ReadFile(hFile, &fScale.y, sizeof(_float), &dwByte, nullptr);

											ReadFile(hFile, &fPosition.x, sizeof(_float), &dwByte, nullptr);
											ReadFile(hFile, &fPosition.y, sizeof(_float), &dwByte, nullptr);

											if (0 == dwByte)
											{
												break;
											}

											m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"));

											m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Com_Transform"), m_iNumItems);
											Safe_AddRef(m_pTransform);

											m_pTransform->Set_Scaled(XMLoadFloat3(&fScale));
											m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y, 0.f, 1.f));

											Safe_Release(m_pTransform);

											m_iNumItems++;
										}
									}
									pItem->Release();
								}
							}
						}
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
	}
}

void CImguiManager::Object_Tool()
{
	_float3 fPosition = { 0.f, 0.f, 0.f };

	m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	XMStoreFloat3(&fPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

	ImGui::Text("Position");
	ImGui::PushItemWidth(50);
	ImGui::InputFloat("Position.x", &fPosition.x); ImGui::SameLine();
	ImGui::InputFloat("Position.y", &fPosition.y); ImGui::SameLine();
	ImGui::InputFloat("Position.z", &fPosition.z);

	if (m_pGameInstance->Get_DIKeyState(DIK_P) & 0x80)
	{
		CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

		fPosition = pTerrain->Get_PickingPosition();
	}

	_float3 fRotation = { 0.f ,0.f ,0.f };

	ImGui::Text("Rotation");

	_vector vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);

	static int e = 0;
	ImGui::RadioButton("Right_Axis", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Up_Axis", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("Look_Axis", &e, 2);

	if (ImGui::Button("Rotate +10"))
	{
		if (e == 0)
			m_pTransform->Turn(vRight, XMConvertToRadians(10));

		else if (e == 1)
			m_pTransform->Turn(vUp, XMConvertToRadians(10));

		else if (e == 2)
			m_pTransform->Turn(vLook, XMConvertToRadians(10));
	}

	if (ImGui::Button("Rotate -10"))
	{
		if (e == 0)
			m_pTransform->Turn(vRight, XMConvertToRadians(-10));

		else if (e == 1)
			m_pTransform->Turn(vUp, XMConvertToRadians(-10));

		else if (e == 2)
			m_pTransform->Turn(vLook, XMConvertToRadians(-10));
	}



	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y, fPosition.z, 1.f));
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