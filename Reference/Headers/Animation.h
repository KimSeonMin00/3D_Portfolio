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
	void Update_TransformationMatrices(_double TimeDelta);

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
	_bool			m_isFinished = false;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END
