#include "..\Public\LandObject.h"
#include "GameInstance.h"
#include "Object_Manager.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLandObject::CLandObject(const CLandObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform * pTransform, _uint iLevelID, const _tchar * pLayerTag, _uint iIndex, const _tchar * pComponentTag)
{
	CObject_Manager*		pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == pObject_Manager)
		return E_FAIL;

	Safe_AddRef(pObject_Manager);

	//CVIBuffer_Terrain* pVIBuffer = (CVIBuffer_Terrain*)pObject_Manager->Get_ComponentPtr(iLevelID, pLayerTag, pComponentTag, iIndex);
	//if (nullptr == pVIBuffer)
	//	return E_FAIL;

	//_float3		vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

	//vPosition.y = pVIBuffer->Get_Height(vPosition) + 0.5f;

	//pTransform->Set_State(CTransform::STATE_POSITION, vPosition);


	Safe_Release(pObject_Manager);

	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

}
