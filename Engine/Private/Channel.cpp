#include "..\Public\Channel.h"
#include "HierarchyNode.h"


CChannel::CChannel()
{
}

HRESULT CChannel::NativeConstruct(aiNodeAnim * pChannel, CHierarchyNode * pHierarchyNode)
{
	m_pHierarchyNode = pHierarchyNode;
	Safe_AddRef(m_pHierarchyNode);

	m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pChannel->mNumPositionKeys);

	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME*		pKeyFrame = new KEYFRAME;
		ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

		if (pChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pChannel->mScalingKeys[i].mTime;
		}

		if (pChannel->mNumRotationKeys > i)
		{
			/*memcpy(&vRotation, &pChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			pKeyFrame->Time = pChannel->mRotationKeys[i].mTime;
		}

		if (pChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pChannel->mPositionKeys[i].mTime;
		}

		memcpy(&pKeyFrame->vScale, &vScale, sizeof(_float3));
		memcpy(&pKeyFrame->vRotation, &vRotation, sizeof(_float4));
		memcpy(&pKeyFrame->vPosition, &vPosition, sizeof(_float3));

		m_KeyFrames.push_back(pKeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(_double TimeAcc)
{
	_vector		vScale, vRotation, vPosition;

	if (TimeAcc >= m_KeyFrames.back()->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (TimeAcc >= m_KeyFrames[m_iCurrentKeyFrame + 1]->Time)
			++m_iCurrentKeyFrame;

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		_double		Ratio = (TimeAcc - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame + 1]->Time - m_KeyFrames[m_iCurrentKeyFrame]->Time);

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pHierarchyNode->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CHierarchyNode * pHierarchyNode)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(pAIChannel, pHierarchyNode)))
	{
		MSGBOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();

	Safe_Release(m_pHierarchyNode);
}

