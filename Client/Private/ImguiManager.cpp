#pragma once

#include "stdafx.h"
#include "..\Public\ImguiManager.h"
#include "GameInstance.h"

#include "UI.h"
#include "Terrain.h"
#include "Player.h"
#include "MapObject.h"

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

	m_pDevice = pDevice;
	m_pDeviceContext = pDevice_Context;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElement);
	if (nullptr == m_pShader)
		return E_FAIL;

	ZeroMemory(&m_vPoint, sizeof(_float3) * 3);

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
			m_iCurrentItemIndex = 0;
			m_eToolList = TOOL_UI;			
		}

		if (ImGui::Button("Object"))
		{
			m_iCurrentItemIndex = 0;
			m_eToolList = TOOL_OBJECT;
		}

		if (ImGui::Button("Navigation"))
		{
			m_iCurrentItemIndex = 0;
			m_eToolList = TOOL_NAVIGATION;
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

		case TOOL_NAVIGATION:
			ImGui::Begin("Navigation_Tool");
			Navigation_Tool(TimeDelta);
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
	if (m_eToolList == TOOL_NAVIGATION)
	{
		if (nullptr == m_pShader)
			return E_FAIL;

		_float4x4		WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

		for (auto& pTriangle : m_Triangle)
		{
			m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
			m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeline::D3DTS_PROJ), sizeof(_float4x4));

			m_pShader->Begin(0);

			if (pTriangle != nullptr)
				pTriangle->Render();
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImguiManager::UI_Tool()
{
	if (ImGui::Button("Create"))
	{
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"));
		m_iNumUI++;
	}

	ImGui::PushItemWidth(100);

	if (m_iNumUI > 0)
	{
		char Item[256] = "";
		sprintf_s(Item, "UI_%d", m_iCurrentItemIndex);

		CurrentItem = Item;
	}

	if (ImGui::BeginCombo("UI", CurrentItem))
	{
		for (int n = 0; n < m_iNumUI; n++)
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

										for (_uint i = 0; i < m_iNumUI; i++)
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

										m_iNumUI = 0;

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

											m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Com_Transform"), m_iNumUI);
											Safe_AddRef(m_pTransform);

											m_pTransform->Set_Scaled(XMLoadFloat3(&fScale));
											m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y, 0.f, 1.f));

											Safe_Release(m_pTransform);

											m_iNumUI++;
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
	ImGui::Text("MapObject_Number");
	ImGui::PushItemWidth(100);
	ImGui::InputInt("MapObject_Index", &m_iObjectIndex, 1, 10);
	if (m_iObjectIndex >= m_iNumObjectIndex)
		m_iObjectIndex = m_iNumObjectIndex - 1;

	if (ImGui::Button("Create"))
	{
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_MapObject"), &m_iObjectIndex);
		m_iNumObject++;
		m_iCurrentItemIndex = m_iNumObject - 1;
	}

	if (ImGui::Button("Delete"))
	{
		if (m_iNumObject > 0)
		{
			CGameObject* pGameObject = m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Map"), m_iCurrentItemIndex);

			if (pGameObject != nullptr)
			{
				Safe_AddRef(pGameObject);
				pGameObject->Set_Dead();
				Safe_Release(pGameObject);
				m_iNumObject--;

				if (m_iCurrentItemIndex == 0)
					m_iCurrentItemIndex = 0;

				else
					m_iCurrentItemIndex--;
			}
		}
	}

	if (m_iNumObject > 0)
	{
		char Item[256] = "";
		sprintf_s(Item, "MapObject_%d", m_iCurrentItemIndex);

		CurrentItem = Item;


		if (ImGui::BeginCombo("MapObject", CurrentItem))
		{
			for (int n = 0; n < m_iNumObject; n++)
			{
				char Item[256] = "";
				sprintf_s(Item, "MapObject_%d", n);

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

		_float fScale = 0.f;
		_float3 fRotation = { 0.f ,0.f ,0.f };
		_float3 fPosition = { 0.f, 0.f, 0.f };

		m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Com_Transform"), m_iCurrentItemIndex);

		if (nullptr == m_pTransform)
			return;

		Safe_AddRef(m_pTransform);

		fScale = m_pTransform->Get_Scaled().x;

		ImGui::Text("Scale");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Scale", &fScale);

		if (fScale <= 0.1f)
			fScale = 0.1f;

		m_pTransform->Set_Scaled(XMVectorSet(fScale, fScale, fScale, 0.f));

		ImGui::Text("Rotation");

		_vector vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
		_vector vUp = m_pTransform->Get_State(CTransform::STATE_UP);
		_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);

		static int e = 0;
		ImGui::RadioButton("Right_Axis", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Up_Axis", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("Look_Axis", &e, 2);

		_float mousemove = 0.f;

		if (ImGui::SliderFloat("Rotate", &mousemove, -1.f, 1.f, "%.2f", 1.f))
		{
			if (e == 0)
				m_pTransform->Turn(vRight, XMConvertToRadians(1) * mousemove);

			else if (e == 1)
				m_pTransform->Turn(vUp, XMConvertToRadians(1) * mousemove);

			else if (e == 2)
				m_pTransform->Turn(vLook, XMConvertToRadians(1) * mousemove);
		}

		if (ImGui::Button("Rotate +1"))
		{
			if (e == 0)
				m_pTransform->Turn(vRight, XMConvertToRadians(1));

			else if (e == 1)
				m_pTransform->Turn(vUp, XMConvertToRadians(1));

			else if (e == 2)
				m_pTransform->Turn(vLook, XMConvertToRadians(1));
		}

		if (ImGui::Button("Rotate -1"))
		{
			if (e == 0)
				m_pTransform->Turn(vRight, XMConvertToRadians(-1));

			else if (e == 1)
				m_pTransform->Turn(vUp, XMConvertToRadians(-1));

			else if (e == 2)
				m_pTransform->Turn(vLook, XMConvertToRadians(-1));
		}

		XMStoreFloat3(&fPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

		ImGui::Text("Position");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Position.x", &fPosition.x); ImGui::SameLine();
		ImGui::InputFloat("Position.y", &fPosition.y); ImGui::SameLine();
		ImGui::InputFloat("Position.z", &fPosition.z);

		if (m_pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON))
		{
			CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

			if (nullptr != pTerrain)
			{
				Safe_AddRef(pTerrain);
				fPosition = pTerrain->Get_PickingPosition();
				Safe_Release(pTerrain);
			}
		}

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y, fPosition.z, 1.f));

		if (ImGui::Button("Initialize"))
		{
			_float3 fScale = m_pTransform->Get_Scaled();

			m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale.x);
			m_pTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale.y);
			m_pTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale.z);
		}

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

										for (_uint i = 0; i < m_iNumObject; i++)
										{
											CMapObject* pMapObject = (CMapObject*)m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Map"), i);
											if (pMapObject != nullptr)
											{
												Safe_AddRef(pMapObject);
												m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Com_Transform"), i);
												if (nullptr != m_pTransform)
												{									
													_int	iMapModelIndex;
													_float4x4 WorldMat;

													iMapModelIndex = pMapObject->Get_ModelIndex();

													Safe_AddRef(m_pTransform);
													XMStoreFloat4x4(&WorldMat, m_pTransform->Get_WorldMatrix());
													Safe_Release(m_pTransform);

													WriteFile(hFile, &iMapModelIndex, sizeof(_int), &dwByte, nullptr);
													WriteFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);									
												}
												Safe_Release(pMapObject);
											}

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

										m_iNumObject = 0;

										if (INVALID_HANDLE_VALUE == hFile)
											return;

										DWORD	dwByte = 0;

										while (true)
										{
											_int	iMapModelIndex;
											_float4x4 WorldMat;

											ReadFile(hFile, &iMapModelIndex, sizeof(_int), &dwByte, nullptr);
											ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

											if (0 == dwByte)
											{
												break;
											}

											m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_MapObject"), &iMapModelIndex);

											m_pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Com_Transform"), m_iNumObject);
											Safe_AddRef(m_pTransform);

											m_pTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4x4(&WorldMat).r[0]);
											m_pTransform->Set_State(CTransform::STATE_UP, XMLoadFloat4x4(&WorldMat).r[1]);
											m_pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat4x4(&WorldMat).r[2]);
											m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&WorldMat).r[3]);

											Safe_Release(m_pTransform);

											m_iNumObject++;
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

void CImguiManager::Navigation_Tool(_float TimeDelta)
{
	m_fInputDelay += TimeDelta;

	ImGui::Text("Point Count : %d", m_iPointCount);

	if (m_pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_RBUTTON))
	{
		if (m_fInputDelay >= 0.2f)
		{

			CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), 0);

			_float3 fPosition = _float3(0.f, 0.f, 0.f);

			if (nullptr != pTerrain)
			{
				Safe_AddRef(pTerrain);
				fPosition = pTerrain->Get_PickingPosition();
				fPosition.y = 0.2f;
				Safe_Release(pTerrain);

				Check_TrianglePoint(fPosition);

				m_vPoint[m_iPointCount++] = fPosition;

				if (m_iPointCount == 3)
				{
					Sort_TrianglePoint();

					CVIBuffer_Triangle* pTriangle = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext, m_vPoint);

					m_Triangle.push_back(pTriangle);
					
					m_iPointCount = 0;

					ZeroMemory(&m_vPoint, sizeof(_float3) * 3);
				}
			}

			m_fInputDelay = 0.f;
		}
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

										for (auto& pTrianglePoints : m_TrianglePoints)
										{
											WriteFile(hFile, &pTrianglePoints->vPos1, sizeof(_float3), &dwByte, nullptr);
											WriteFile(hFile, &pTrianglePoints->vPos2, sizeof(_float3), &dwByte, nullptr);
											WriteFile(hFile, &pTrianglePoints->vPos3, sizeof(_float3), &dwByte, nullptr);
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

										m_iNumObject = 0;

										if (INVALID_HANDLE_VALUE == hFile)
											return;

										DWORD	dwByte = 0;

										while (true)
										{
											_float3	vPos1, vPos2, vPos3 = { 0.f ,0.f, 0.f };
											_float3 vPoints[3];
											ZeroMemory(&vPoints, sizeof(_float3) * 3);

											ReadFile(hFile, &vPos1, sizeof(_float3), &dwByte, nullptr);
											ReadFile(hFile, &vPos2, sizeof(_float3), &dwByte, nullptr);
											ReadFile(hFile, &vPos3, sizeof(_float3), &dwByte, nullptr);

											if (0 == dwByte)
											{
												break;
											}

											TRIANGLE* TrianglePoints = new TRIANGLE;

											TrianglePoints->vPos1 = vPos1;
											TrianglePoints->vPos2 = vPos2;
											TrianglePoints->vPos3 = vPos3;

											m_TrianglePoints.push_back(TrianglePoints);

											vPoints[0] = vPos1;
											vPoints[1] = vPos2;
											vPoints[2] = vPos3;

											CVIBuffer_Triangle* pTriangle = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext, vPoints);

											m_Triangle.push_back(pTriangle);
							
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

_bool CImguiManager::Check_TrianglePoint(_float3& fPos)
{
	for (auto& TrianglePoint : m_TrianglePoints)
	{
		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&fPos) - XMLoadFloat3(&(TrianglePoint->vPos1)))) <= 0.5f)
		{
			fPos = TrianglePoint->vPos1;
			return true;
		}

		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&fPos) - XMLoadFloat3(&(TrianglePoint->vPos2)))) <= 0.5f)
		{
			fPos = TrianglePoint->vPos2;
			return true;
		}

		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&fPos) - XMLoadFloat3(&(TrianglePoint->vPos3)))) <= 0.5f)
		{
			fPos = TrianglePoint->vPos3;
			return true;
		}
	}

	return false;
}

void CImguiManager::Sort_TrianglePoint()
{
	TRIANGLE* TrianglePoints = new TRIANGLE;

	_float3 vPosZmax = _float3(0.f, 0.f, 0.f);
	_float3 vPosXmax = _float3(0.f, 0.f, 0.f);
	_float3 vPosLast = _float3(0.f, 0.f, 0.f);

	_uint	iZmaxIndex = 0;
	_uint	iXmaxIndex = 0;
	_uint	iLastIndex = 0;

	for (int i = 0; i < 3; i++)
	{
		if (m_vPoint[i].z >= vPosZmax.z)
		{
			vPosZmax = m_vPoint[i];
			iZmaxIndex = i;
		}
	}

	for (int j = 0; j < 3; j++)
	{
		if (j == iZmaxIndex)
			continue;
		else
		{
			if (m_vPoint[j].x >= vPosXmax.x)
			{
					vPosXmax = m_vPoint[j];
					iXmaxIndex = j;
			}
		}
	}

	for (int k = 0; k < 3; k++)
	{
		if (k != iXmaxIndex && k != iZmaxIndex)
			iLastIndex = k;
	}

	_vector vSrc;
	_vector vDest;

	if (m_vPoint[iZmaxIndex].x > m_vPoint[iXmaxIndex].x)
	{
		vSrc = XMVector3Normalize(XMLoadFloat3(&m_vPoint[iXmaxIndex]) - XMLoadFloat3(&m_vPoint[iZmaxIndex]));
		vDest = XMVector3Normalize(XMVectorSet(m_vPoint[iLastIndex].z - m_vPoint[iZmaxIndex].z, 0.f, -m_vPoint[iLastIndex].x + m_vPoint[iZmaxIndex].x, 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vSrc, vDest)))
		{
			TrianglePoints->vPos1 = m_vPoint[iZmaxIndex];
			TrianglePoints->vPos2 = m_vPoint[iLastIndex];
			TrianglePoints->vPos3 = m_vPoint[iXmaxIndex];
		}

		else
		{
			TrianglePoints->vPos1 = m_vPoint[iZmaxIndex];
			TrianglePoints->vPos2 = m_vPoint[iXmaxIndex];
			TrianglePoints->vPos3 = m_vPoint[iLastIndex];
		}
	}

	else
	{
		vSrc = XMVector3Normalize(XMLoadFloat3(&m_vPoint[iLastIndex]) - XMLoadFloat3(&m_vPoint[iZmaxIndex]));
		vDest = XMVector3Normalize(XMVectorSet(-m_vPoint[iXmaxIndex].z + m_vPoint[iZmaxIndex].z, 0.f, m_vPoint[iXmaxIndex].x - m_vPoint[iZmaxIndex].x, 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vSrc, vDest)))
		{
			TrianglePoints->vPos1 = m_vPoint[iZmaxIndex];
			TrianglePoints->vPos2 = m_vPoint[iLastIndex];
			TrianglePoints->vPos3 = m_vPoint[iXmaxIndex];
		}

		else
		{
			TrianglePoints->vPos1 = m_vPoint[iZmaxIndex];
			TrianglePoints->vPos2 = m_vPoint[iXmaxIndex];
			TrianglePoints->vPos3 = m_vPoint[iLastIndex];
		}
	}

	m_TrianglePoints.push_back(TrianglePoints);
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

	for (auto& pTrianglePoint : m_TrianglePoints)
		Safe_Delete(pTrianglePoint);

	m_TrianglePoints.clear();

	for (auto& pTriangle : m_Triangle)
		Safe_Release(pTriangle);

	m_Triangle.clear();

	if (m_pTransform != nullptr)
		Safe_Release(m_pTransform);

	Safe_Release(m_pShader);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}