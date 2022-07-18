#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CModel;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

class CWhirlWind abstract
	:public CGameObject
{
protected:
	explicit CWhirlWind(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CWhirlWind(const CWhirlWind& rhs);
	virtual ~CWhirlWind() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CCollider*						m_pSPHERECom = nullptr;
	CTexture*						m_pTextureAlpha = nullptr;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

