#include "..\Public\Pipeline.h"


IMPLEMENT_SINGLETON(CPipeline)

CPipeline::CPipeline()
{
}

void CPipeline::Tick()
{
	for (_uint i = 0; i < D3DTS_END; ++i)
		XMStoreFloat4x4(&m_TransposeMatrix[i],
			XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix[i])));

	XMStoreFloat4(&m_vCamPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_VIEW])).r[3]);
}

void CPipeline::Free()
{
}
