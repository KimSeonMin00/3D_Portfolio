#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;


public:
	_uint Get_Depth() const {
		return m_iDepth;
	}

	const char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_OffsetMatrix() const {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public:
	void Set_OffsetMatrix(const _float4x4* pOffsetMatrix);
	void Set_TransformationMatrix(_fmatrix TransformMatrix);


public:
	HRESULT NativeConstruct(const char* pBoneName, _float4x4 TransformMatrix, CHierarchyNode* pParent, _uint iDepth);

public:
	void Update_CombinedTransformationMatrix();

private:
	char			m_szName[MAX_PATH] = "";
	_float4x4		m_OffsetMatrix;
	_float4x4		m_TransformationMatrix;
	_float4x4		m_CombinedTransformationMatrix;
	CHierarchyNode*	m_pParent = nullptr;
	_uint			m_iDepth = 0;

public:
	static CHierarchyNode* Create(const char* pBoneName, _float4x4 TransformMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free();
};

END
