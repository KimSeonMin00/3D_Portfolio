#pragma once


namespace Engine
{
	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	}KEYFRAME;

	typedef struct tagModelMaterial
	{
		class CTexture*		pTexture[AI_TEXTURE_TYPE_MAX];
	}MODELMATERIAL;

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

	typedef struct tagVertex_Anim
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;
		XMFLOAT4			vBlendWeight;
	}VTXANIM;

	struct ENGINE_DLL VTXANIM_DECLARATION
	{
		static const unsigned int iNumElement = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};

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

	typedef struct tagVertexColor
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXCOL;

	struct ENGINE_DLL VTXCOL_DECLARATION
	{
		static const unsigned int iNumElement = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};

	typedef struct tagVertexMatrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
	}VTXMATRIX;

	struct ENGINE_DLL VTXTEXINSTANCE_DECLARATION
	{
		static const unsigned int iNumElement = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};

	typedef struct tagVertexPoint
	{
		XMFLOAT3			vPosition;
		float				fPSize;
	}VTXPOINT;

	struct ENGINE_DLL VTXPOINTINSTANCE_DECLARATION
	{
		static const unsigned int iNumElement = 6;
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

