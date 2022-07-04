#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	void Set_CurrentKeyFrameIndex(_uint iIndex) {
		m_iCurrentKeyFrame = iIndex;
	}

	const char* Get_Name() const {
		return m_szName;
	}

	void Set_HierarchyNode(class CHierarchyNode* pNode) {
		m_pHierarchyNode = pNode;
		Safe_AddRef(m_pHierarchyNode);
	}

	_uint Get_KeyFrame()
	{
		return m_iCurrentKeyFrame;
	};

	KEYFRAME* Get_CurrentKeyFrameInfo()
	{
		return m_KeyFrames[m_iCurrentKeyFrame];
	};
	class CHierarchyNode* Get_Bone()
	{
		return m_pHierarchyNode;
	};

public:
	HRESULT NativeConstruct(aiNodeAnim* pChannel, class CHierarchyNode* pHierarchyNode);
	void Update_TransformationMatrix(_double TimeAcc);

private:
	char				m_szName[MAX_PATH] = "";

private:
	_uint						m_iCurrentKeyFrame = 0;
	_uint						m_iNumKeyFrames = 0;
	vector<KEYFRAME*>			m_KeyFrames;
	typedef vector<KEYFRAME*>	KEYFRAMES;

private:
	class CHierarchyNode*		m_pHierarchyNode = nullptr;
	_bool						m_isCloned = false;

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, CHierarchyNode* pHierarchyNode);
	CChannel* Clone();
	virtual void Free() override;
};

END
