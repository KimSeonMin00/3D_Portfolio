#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "UI.h"
#include "Player.h"
#include "WhirlWind_Normal.h"
#include "WhirlWind_EQ.h"
#include "Volibear.h"
#include "Effect_Voli_E.h"
#include "MapObject.h"

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
		CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhirlWind_Normal"),
		CWhirlWind_Normal::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhirlWind_EQ"),
		CWhirlWind_EQ::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss"),
		CVolibear::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_E"),
		CEffect_Voli_E::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(10.f, XMConvertToRadians(60.f))))))
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
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Meshes/Map/earth_river_dragoncamp_ground_a.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Voli_E"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_e_0%d.dds"), 2))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Player. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/AKIHA_AKI00_00%d.png"), 12))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_SkyBox. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyBox"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));
	/* For.Prototype_Component_Model_Fiona. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yasuo"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Yasuo.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Volibear"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Volibear.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	Load_MapObject(LEVEL_GAMEPLAY);

	lstrcpy(m_szLoadingText, TEXT("콜라이더를 생성 중입니다. "));
	/* For.Prototype_Component_Collider_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다. "));

	/* For.Prototype_Component_Navigation*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation_Boss.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을 생성 중입니다. "));
	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

#pragma endregion

	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Load_MapObject(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_0"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_0.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_1.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_2.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_3.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_4.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_5"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_5.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_6"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_6.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_7"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_7.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_8"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_8.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_9"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_9.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_10"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_10.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_11"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_11.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_12"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_12.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_13"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_13.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_14"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_14.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_15"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_15.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_16"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_16.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_17"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_17.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_18"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_18.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_19"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_19.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_20"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_20.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_21"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_21.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_22"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_22.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_23"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_23.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_24"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_24.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_25"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_25.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_26"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_26.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_27"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_27.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_28"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_28.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_29"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_29.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_30"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_30.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_31"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_31.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_32"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_32.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_33"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_33.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_34"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_34.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_35"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_35.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_36"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_36.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_37"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_37.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_38"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_38.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_39"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_39.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_40"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_40.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_41"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_41.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_42"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_42.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_43"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_43.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_44"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_44.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_45"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_45.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_46"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_46.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_47"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_47.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_48"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_48.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_49"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_49.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_50"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_50.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_51"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_51.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_52"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_52.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_53"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_53.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_54"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_54.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_55"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_55.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_56"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_56.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_57"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_57.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_58"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_58.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_59"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_59.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_60"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_60.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_61"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_61.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_62"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_62.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_63"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_63.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_64"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_64.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_65"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_65.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_66"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_66.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_67"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_67.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_68"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_68.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_69"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_69.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_70"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_70.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_71"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_71.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_72"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_72.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_73"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_73.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_74"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_74.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_75"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_75.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_76"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_76.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_77"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_77.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_78"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_78.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_79"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_79.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_80"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_80.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_81"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_81.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_82"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_82.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_83"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_83.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_84"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_84.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_85"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_85.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_86"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_86.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_87"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_87.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_88"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_88.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_89"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_89.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_90"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_90.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_91"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_91.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_92"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_92.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_93"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_93.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_94"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_94.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_95"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_95.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_96"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_96.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_97"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_97.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_98"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_98.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_99"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_99.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_100"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_100.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_101"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_101.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_102"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_102.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_103"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_103.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_104"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_104.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_105"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_105.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_106"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_106.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_107"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_107.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_108"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_108.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_109"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_109.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_110"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_110.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_111"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_111.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_112"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_112.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_113"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_113.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_114"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_114.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_115"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_115.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_116"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_116.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_117"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_117.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_118"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_118.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_119"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_119.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_120"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_120.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_121"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_121.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_122"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_122.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_123"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_123.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_124"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_124.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_125"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_125.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_126"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_126.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_127"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_127.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_128"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_128.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_129"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_129.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_130"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_130.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_131"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_131.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_132"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_132.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_133"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_133.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_134"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_134.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_135"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_135.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_136"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_136.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_137"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_137.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_138"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_138.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_139"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_139.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_140"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_140.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_141"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_141.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_142"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_142.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_143"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_143.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_144"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_144.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_145"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_145.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_146"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_146.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_147"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_147.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_148"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_148.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_149"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_149.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_150"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_150.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_151"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_151.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_152"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_152.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_153"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_153.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_154"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_154.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_155"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_155.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_156"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_156.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_157"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_157.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_158"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_158.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_159"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_159.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_160"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_160.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_161"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_161.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_162"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_162.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_163"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_163.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_164"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_164.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_165"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_165.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_166"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_166.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_167"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_167.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_168"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_168.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_169"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_169.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_170"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_170.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_171"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_171.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_172"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_172.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_173"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_173.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_174"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Map/", "MapObject_174.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
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
