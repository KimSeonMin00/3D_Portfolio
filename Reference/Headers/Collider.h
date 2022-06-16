#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3		vScale;
		_float3		vAngle;
		_float3		vPosition;
	}COLLIDERDESC;
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG



private:
	TYPE					m_eType = TYPE_END;

	BoundingBox*			m_pAABB_World = nullptr;
	BoundingOrientedBox*	m_pOBB_World = nullptr;
	BoundingSphere*			m_pSPHERE_World = nullptr;

	BoundingBox*			m_pAABB = nullptr;
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingSphere*			m_pSPHERE = nullptr;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*			m_pBatch = nullptr;
	BasicEffect*									m_pEffect = nullptr;
	ID3D11InputLayout*								m_pInputLayout = nullptr;
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix Transform);


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

