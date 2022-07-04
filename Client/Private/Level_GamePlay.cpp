#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"

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

	/*if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;*/


	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	

	


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
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

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss"))))
		return E_FAIL;

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
