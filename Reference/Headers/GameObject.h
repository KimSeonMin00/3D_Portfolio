#pragma once

#include "Transform.h"

/* 인게임에 출현할 수 있는 게임오브젝트들의 부모다. */
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;	

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

protected:
	static const _tchar*		m_pTransformTag;
	class CTransform*			m_pTransformCom = nullptr;

protected:
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

public:
	/* 부모의 컨테이너에도 보관. 자식의 멤버변수에도 보관. */
	HRESULT Add_Components(_uint iLevelID, const _tchar * pPrototypeTag, const _tchar* pComponentTag, CComponent ** ppOut, void * pArg = nullptr);


	

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END