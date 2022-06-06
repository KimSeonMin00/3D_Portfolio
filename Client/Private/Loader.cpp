#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "UI.h"
#include "Player.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY EntryFunction(void* pArg)
{
	CLoader*			pLoader = (CLoader*)pArg;
	
	EnterCriticalSection(&pLoader->Get_CriticalSection());

	_uint		iProgress = 0;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		iProgress = pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		iProgress = pLoader->Loading_ForGamePlay();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return iProgress;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_uint CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	///* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;

	///* For.Prototype_GameObject_UI */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
	//	CUI::Create(m_pGraphic_Device))))
	//	return -1;

#pragma endregion


#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Default%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 생성중입니다."));

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	///* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;


	///*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1000.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return -1;



#pragma endregion

#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));

	///* For.Prototype_Component_VIBuffer_Terrain. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Default. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Default"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;


	///* For.Prototype_Component_Texture_Terrain . */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Player. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/AKIHA_AKI00_00%d.png"), 12))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_SkyBox. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyBox"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));
	/* For.Prototype_Component_Model_Fiona. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yasuo"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Fiona.fbx", CModel::TYPE_NONANIM))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을 생성 중입니다. "));
	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

#pragma endregion

	Safe_Release(pGameInstance);

	return _uint();
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevelID)))
	{
		MSGBOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	
	CloseHandle(m_hThread);
}
