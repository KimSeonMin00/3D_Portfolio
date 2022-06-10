#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(aiAnimation * pAIAnimation, CModel * pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CHierarchyNode*		pHierarhcyNode = pModel->Find_HierarcyNodes(pAIAnimation->mChannels[i]->mNodeName.data);

		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pHierarhcyNode);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrices(_double TimeDelta)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		m_TimeAcc = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Set_CurrentKeyFrameIndex(0);

		m_Channels[i]->Update_TransformationMatrix(m_TimeAcc);
	}

	if (true == m_isFinished)
		m_isFinished = false;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel * pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pAIAnimation, pModel)))
	{
		MSGBOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
