#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	void Set_CurrentKeyFrameIndex(_uint iIndex) {
		m_iCurrentKeyFrame = iIndex;
	}

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

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, CHierarchyNode* pHierarchyNode);
	virtual void Free() override;
};

END
