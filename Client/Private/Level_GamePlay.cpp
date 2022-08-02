#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Level_Loading.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;*/


	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*if (m_bBossSpawned == false)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_Layer_Size(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		if (iLayerSize == 0)
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Pantheon"))))
				return;

			m_bBossSpawned = true;
		}

		RELEASE_INSTANCE(CGameInstance);	
	}

	else
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_Layer_Size(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		if (iLayerSize == 0)
		{
			if(pGameInstance->Get_DIKeyState(DIK_RETURN) & 0x80)
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_BOSS))))
					return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}*/
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	/*LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vPosition = _float4(15.f, 5.f, 15.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	return E_FAIL;*/

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;


	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss"))))
		return E_FAIL;*/

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Pantheon"))))
		return E_FAIL;*/

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_MapObject"))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);
	//HANDLE		hFile = CreateFile(TEXT("../Bin/Data/TestMap.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/TestMap.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD	dwByte = 0;

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_int iNumObject = 0;

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

		pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_MapObject"), &iMapModelIndex);

		CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, pLayerTag, TEXT("Com_Transform"), iNumObject);
		Safe_AddRef(pTransform);

		pTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4x4(&WorldMat).r[0]);
		pTransform->Set_State(CTransform::STATE_UP, XMLoadFloat4x4(&WorldMat).r[1]);
		pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat4x4(&WorldMat).r[2]);
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&WorldMat).r[3]);

		Safe_Release(pTransform);

		iNumObject++;
	}

	CloseHandle(hFile);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Bird"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Owl"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Duck"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Duckie"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Monster_Test.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD	dwByte = 0;

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_int iNumMonster = 0;

	while (true)
	{
		_int	iMonsterIndex;
		_float4x4 WorldMat;

		ReadFile(hFile, &iMonsterIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}

		if (iMonsterIndex == 0)
			pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Wolf"));

		else if (iMonsterIndex == 1)
			pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RazorBeak"));

		else if (iMonsterIndex == 2)
			pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Red"));

		CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, pLayerTag, TEXT("Com_Transform"), iNumMonster);
		Safe_AddRef(pTransform);

		pTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4x4(&WorldMat).r[0]);
		pTransform->Set_State(CTransform::STATE_UP, XMLoadFloat4x4(&WorldMat).r[1]);
		pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat4x4(&WorldMat).r[2]);
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&WorldMat).r[3]);

		Safe_Release(pTransform);

		iNumMonster++;
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;	
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
