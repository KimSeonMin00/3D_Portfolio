#include "..\Public\GameObject.h"

#include "GameInstance.h"
#include "Transform.h"

const _tchar* CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pTransformCom((CTransform*)rhs.m_pTransformCom->Clone(nullptr))
{	
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext, TransformDesc);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	m_Components.emplace(m_pTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	m_iLevel = pGameInstance->Get_NextLevel();

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{

}

void CGameObject::Late_Tick(_float fTimeDelta)
{

}

HRESULT CGameObject::Render()
{

	return S_OK;
}

HRESULT CGameObject::Render_ShadowDepth()
{
	return S_OK;
}

HRESULT CGameObject::Add_Components(_uint iLevelID, const _tchar * pPrototypeTag, const _tchar* pComponentTag, CComponent ** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelID, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	m_Components.emplace(pComponentTag, pComponent);

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
