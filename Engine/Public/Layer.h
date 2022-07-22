#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag, _uint iIndex);
	class CGameObject* Get_GameObjectPtr(_uint iIndex);
public:
	HRESULT NativeConstruct();

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	_uint Get_Size();
private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END