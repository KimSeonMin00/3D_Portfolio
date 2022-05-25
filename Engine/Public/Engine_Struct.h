#pragma once


namespace Engine
{
	typedef struct tagVertexTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	struct ENGINE_DLL VTXTEX_DECLARATION
	{
		static const unsigned int iNumElement = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;
}

