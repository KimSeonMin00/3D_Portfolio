#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)
class CMapObject_Anim abstract
	:public CGameObject
{
protected:
	explicit CMapObject_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CMapObject_Anim(const CMapObject_Anim& rhs);
	virtual ~CMapObject_Anim() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	Set_Select()
	{
		m_bSelected = true;
	}

private:
	_bool	m_bSelected = false;

protected:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
