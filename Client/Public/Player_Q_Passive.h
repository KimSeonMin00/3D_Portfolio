#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CPlayer_Q_Passive final
	:public CGameObject
{
protected:
	explicit CPlayer_Q_Passive(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPlayer_Q_Passive(const  CPlayer_Q_Passive& rhs);
	virtual ~CPlayer_Q_Passive() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom_1 = nullptr;
	CModel*						m_pModelCom_2 = nullptr;
	CTexture*					m_pTexture_PassiveWind1 = nullptr;
	CTexture*					m_pTexture_PassiveWind2 = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable_Model_1(_uint iNumModel);
	HRESULT SetUp_ConstantTable_Model_2(_uint iNumModel);

public:
	static CPlayer_Q_Passive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END