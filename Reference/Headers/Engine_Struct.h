#pragma once


namespace Engine
{
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		XMFLOAT4		vDirection;

		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;


	typedef struct tagMaterial
	{
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}MATERIALDESC;

	typedef struct tagVertex_NonAnim
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXNONANIM;

	struct ENGINE_DLL VTXNONANIM_DECLARATION
	{
		static const unsigned int iNumElement = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};

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

	typedef struct tagVertexNormalTexture
	{

		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vPosition;
	}VTXNORTEX;

	struct ENGINE_DLL VTXNORTEX_DECLARATION
	{
		static const unsigned int iNumElement = 3;
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

