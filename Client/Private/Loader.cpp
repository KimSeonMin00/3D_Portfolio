#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"

#include "UI.h"
#include "Player_Hit_UI.h"
#include "Player_HP.h"
#include "FadeOut.h"
#include "Stun.h"

#include "Player.h"
#include "WhirlWind_Normal.h"
#include "Player_Q_Dirt.h"
#include "WhirlWind_EQ.h"
#include "WhirlWind_Hit_Effect.h"
#include "Player_E_AfterImage.h"
#include "Player_E_Effect.h"
#include "Player_Q_Effect.h"
#include "Player_Q_Hit_Effect.h"
#include "Player_Q_Passive.h"
#include "Player_R_Effect.h"
#include "Player_R_Hit_Effect.h"
#include "Player_Attack_Effect.h"

#include "Boss_HP.h"
#include "Volibear.h"
#include "Voli_Ghost.h"
#include "Effect_Voli_E.h"
#include "Voli_Passive_Effect.h"
#include "Voli_Q_Footprint.h"
#include "Voli_Q_Down.h"
#include "Voli_R_Effect.h"
#include "Voli_Hit_Effect.h"

#include "Pantheon.h"
#include "Pantheon_Q_Effect.h"
#include "Pantheon_Q_Spear.h"
#include "Pantheon_Shield.h"
#include "Pantheon_E_Slash.h"
#include "Pantheon_E_Swipe.h"
#include "Pantheon_W_Effect.h"
#include "Pantheon_Hit_Effect.h"
#include "Pantheon_Q_Hit_Effect.h"

#include "Hit_Effect_Normal.h"
#include "Monster_HP.h"
#include "Wolf.h"
#include "Red.h"
#include "RazorBeak.h"

#include "MapObject.h"
#include "MapObject_Anim.h"
#include "Bird.h"
#include "Owl.h"
#include "Duck.h"
#include "Duckie.h"

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

	case LEVEL_BOSS:
		iProgress = pLoader->Loading_ForBoss();
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

_uint CLoader::Create_Prototype()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	///* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stun"),
		CStun::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	///*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhirlWind_Normal"),
		CWhirlWind_Normal::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_Q_Dirt"),
		CPlayer_Q_Dirt::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhirlWind_EQ"),
		CWhirlWind_EQ::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WirlWind_Hit_Effect"),
		CWhirlWind_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_E_AF"),
		CPlayer_E_AfterImage::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_E_Effect"),
		CPlayer_E_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_Q_Effect"),
		CPlayer_Q_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_Q_Hit_Effect"),
		CPlayer_Q_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_Q_Passive"),
		CPlayer_Q_Passive::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_R_Effect"),
		CPlayer_R_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_R_Hit_Effect"),
		CPlayer_R_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yasuo_Attack_Effect"),
		CPlayer_Attack_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_HP"),
		CPlayer_HP::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_HP"),
		CBoss_HP::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss"),
		CVolibear::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Voli_Ghost"),
		CVoli_Ghost::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_E"),
		CEffect_Voli_E::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_Passive"),
		CVoli_Passive_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_Q_Footprint"),
		CVoli_Q_Footprint::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_Q_Down"),
		CVoli_Q_Down::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_R"),
		CVoli_R_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Voli_Hit"),
		CVoli_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	//
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon"),
		CPantheon::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_Q_Effect"),
		CPantheon_Q_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_Q_Spear"),
		CPantheon_Q_Spear::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_E_Shield"),
		CPantheon_Shield::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_E_Slash"),
		CPantheon_E_Slash::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_E_Swipe"),
		CPantheon_E_Swipe::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_W_Effect"),
		CPantheon_W_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_Hit_Effect"),
		CPantheon_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pantheon_Q_Hit_Effect"),
		CPantheon_Q_Hit_Effect::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;
	//
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hit_Effect_Normal"),
		CHit_Effect_Normal::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_HP"),
		CMonster_HP::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wolf"),
		CWolf::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Red"),
		CRed::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RazorBeak"),
		CRazorBeak::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bird"),
		CBird::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Owl"),
		COwl::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Duck"),
		CDuck::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Duckie"),
		CDuckie::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(60.f))))))
		return -1;

	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(10.f, XMConvertToRadians(60.f))))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Hit_UI"),
		CPlayer_Hit_UI::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeOut"),
		CFadeOut::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return -1;

#pragma endregion
	Safe_Release(pGameInstance);

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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Loading_%d.dds"), 3))))
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
	Create_Prototype();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));

	///* For.Prototype_Component_VIBuffer_Terrain. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Terrain . */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Meshes/Map/earth_river_dragoncamp_ground_a.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Stun"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Stun.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다. "));

	/* For.Prototype_Component_Navigation*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Default. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Default"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;
	
	///* For.Prototype_Component_Texture_SkyBox. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyBox"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	Load_Collider(LEVEL_GAMEPLAY);

	Load_Shader(LEVEL_GAMEPLAY);

	Load_Player(LEVEL_GAMEPLAY);

	Load_UI(LEVEL_GAMEPLAY);

	Load_Monster(LEVEL_GAMEPLAY);

	Load_Pantheon(LEVEL_GAMEPLAY);

	Load_MapObject(LEVEL_GAMEPLAY);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

_uint CLoader::Loading_ForBoss()
{
	//Create_Prototype();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));

	///* For.Prototype_Component_VIBuffer_Terrain. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Terrain . */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Meshes/Map/earth_river_dragoncamp_ground_a.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Stun"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Stun.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다. "));

	/* For.Prototype_Component_Navigation*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation_Boss.dat")))))
		return E_FAIL;

	Load_Collider(LEVEL_BOSS);

	Load_Shader(LEVEL_BOSS);

	Load_Player(LEVEL_BOSS);

	Load_UI(LEVEL_BOSS);

	Load_VoliBear(LEVEL_BOSS);

	Load_MapObject(LEVEL_BOSS);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

_uint CLoader::Load_Collider(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("콜라이더를 생성 중입니다. "));
	/* For.Prototype_Component_Collider_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_uint CLoader::Load_Shader(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("셰이더을 생성 중입니다. "));
	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
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

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Bird"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "bird.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Mouse"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "Mouse.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Owl"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "owl.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Lizard"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "lizard.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Duck"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "duck.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_MapObject_Duckie"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/MapObject/", "duckie.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_uint CLoader::Load_Player(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//For Player_Effect
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Tornado_Alpha"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_q_tonado_wind_cas.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Tornado_2_Alpha"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_e_tonado_wind_cas.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Yasuo_Q_Indicator"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_indicator.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Yasuo_Q_Sword"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_sword.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_E_Q_Black"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_e_dash_black.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_E_Q_White"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_e_dash_white.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Yasuo_Passive_Q_1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_tornado_buff.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Yasuo_Passive_Q_2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_tornado_buff_af.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_Blast"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_wind_blast.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Dash_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_e_dash_flash.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_String"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_land_impact_string.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_Ring"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_land_exp_ring_af.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Q_Crack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_tornado_ground_shape_cas.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Q_Ground_Crack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_q_groud_crack.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_Crack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_ground_crack.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Attack_Slash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_basic_attack_impact_slash.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Attack_Wind"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_basic_attack_wind_ring_02.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Attack_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_ba_crit_flare.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Q_Dirt"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_q_ground_dirt_mis_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Q_Hit"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_tornado_blast.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Q_Hit_Tar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_q_hit_spark_tar_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Yasuo_E_Ring"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_e_dash_ring_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_hit_flash_tar_2.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_R_Blood"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_r_hit_streak_tar_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Tornado_Smoke"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_passive_smoke_%d.dds"), 15))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Tornado_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_base_q_wind_hit_flash.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_Smoke"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Player/yasuo_q_smoke_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_HP_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/Player_HP_Frame.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Player_HP"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/Player_HP_Color.dds")))))
		return E_FAIL;

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));
	/* For.Prototype_Component_Model_Fiona. */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Yasuo"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Yasuo.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Tornado"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "Q_Tornado.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Tornado_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "Q_Tornado_2.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_E_Q_Slash"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "e_q_slash_black.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Yasuo_Passive_Q"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "q_windPassive.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Player_R_Blast"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "r_ground_blast.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Player_Q_Rock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "q_ground_rock.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Player_E_AF"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Effect/", "e_AfterImage.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

_uint CLoader::Load_VoliBear(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Voli_E"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_e_0%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_Passive_Spark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_sparkmote_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_Passive_Tar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_passive_tar_flipbook_%d.dds"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_flash_01.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_Q_Crack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_skin16_crater_01.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_Lightening"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_e_skybolt_01.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_R_Warning"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_r_aoe_warning.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Voli_R_Crator"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Volibear/Effect/volibear_base_r_landingcrater_add.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Monster_HP_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/monster_HP_Frame.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Monster_HP"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/monster_HP_Color.dds")))))
		return E_FAIL;

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Volibear"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Volibear.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

_uint CLoader::Load_Pantheon(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Pantheon"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/", "Pantheon.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Pantheon_E_Wing"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Pantheon/", "Shield_Wing.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Pantheon_E_Center"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Pantheon/", "Shield_Center.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Pantheon_E_Swipe"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Pantheon/", "pantheon_e_Swipe.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Pantheon_Q_Spear"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Pantheon/", "Pantheon_Spear.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_q_tap_spear_sharper.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Spear_Blur"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_q_tap_spearhead_blur.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Trail"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_air_trail.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Trail_Center"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_air_trail_center.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Slash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_swipe_tar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Indicator"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_swipe_aoe_indicator.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_E_Fill"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_trapezoid_fill.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_W_Slam"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_shield_slam_glow.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_W_Crack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_ground_cracks.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Fire"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_fire_mult_%d.dds"), 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Hit_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin_06_tar_flash.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Hit_Slash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_hit_flash_spikes_yellow.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Hit_Flash"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_erodeout.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Hit_Flare"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_impact_flare.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Hit_Cross"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_q_cross_stars.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Pantheon_Q_Hit_Spark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Pantheon/pantheon_skin06_flash_orange.dds")))))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

_uint CLoader::Load_Monster(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Wolf"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Monster/", "Wolf.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Red"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Monster/", "Red.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_RazorBeak"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Monster/", "RazorBeak.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Monster_Hit"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/Monster/monster_hit_blast.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Monster_HP_Frame"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/monster_HP_Frame.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Monster_HP"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/monster_HP_Color.dds")))))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

_uint CLoader::Load_UI(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Hit_UI"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Texture/UI/lol_ingame_hit.dds")))))
		return E_FAIL;

	Safe_Release(pGameInstance);
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
