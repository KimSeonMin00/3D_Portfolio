#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct(aiAnimation * pAIAnimation, class CModel* pModel);
	void Check_Looped(_double TimeDelta);
	void Update_TransformationMatrices(_double TimeDelta);

public:
	void			Set_Initialize();
	_bool			Get_Finished()
	{
		return m_isFinished;
	};
	_uint			Get_KeyFrame();
	KEYFRAME*		Get_CurrentKeyFrameInfo(_uint ChannelIndex);
	_uint			Get_NumChannels()
	{
		return m_iNumChannels;
	};

	class CHierarchyNode* Get_Bone(_uint iChannelIndex);

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = 0.0;
	_double			m_TickPerSecond = 0.0;

private:
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;
	typedef vector<class CChannel*>		CHANNELS;

private:
	_double			m_TimeAcc = 0.0;
	_bool				m_isFinished = false;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END
