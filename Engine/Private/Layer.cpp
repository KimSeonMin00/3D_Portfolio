#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);
}

CGameObject * CLayer::Get_GameObjectPtr(_uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter);
}

HRESULT CLayer::NativeConstruct()
{

	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	auto& iter = m_GameObjects.begin();

	for (; iter != m_GameObjects.end();)
	{
		if ((*iter)->Check_Dead())
		{
			Safe_Release((*iter));
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			(*iter)->Late_Tick(fTimeDelta);
			iter++;
		}

	}
}

_uint CLayer::Get_Size()
{
	return m_GameObjects.size();
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();

}

