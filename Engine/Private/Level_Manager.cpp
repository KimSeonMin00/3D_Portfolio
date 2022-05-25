#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

HRESULT CLevel_Manager::Open_Level(_uint iLevelID, CLevel * pNewLevel)
{
	if (nullptr == pNewLevel)
		return E_FAIL;

	/* 이전 레벨용도의 오브젝트들을 정리한다. */
	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Clear(m_iLevelID)))
			return E_FAIL;

		Safe_Release(pGameInstance);

		Safe_Release(m_pCurrentLevel);
	}		

	m_pCurrentLevel = pNewLevel;

	m_iLevelID = iLevelID;

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
