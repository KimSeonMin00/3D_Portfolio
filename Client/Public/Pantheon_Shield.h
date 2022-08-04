#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)
class CPantheon_Shield
	:public CGameObject
{
protected:
	explicit CPantheon_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CPantheon_Shield(const CPantheon_Shield& rhs);
	virtual ~CPantheon_Shield() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	Set_Pos(_vector vPos)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Go_Left(2.f);
	}

private:
	_float		m_fLiveTime = 0.f;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModel_Wing = nullptr;
	CModel*							m_pModel_Center = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPantheon_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
