#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_LightViewMatrix, g_LightProjMatrix;

float			g_Alpha = 1.f;

struct BoneDesc
{
	matrix		BoneMatrices[180];
};

BoneDesc		g_Bones;
texture2D		g_DiffuseTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(In.vNormal, g_WorldMatrix).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = vPosition;

	return Out;
}

VS_OUT VS_LIGHT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matLWV, matLWVP;

	matLWV = mul(g_WorldMatrix, g_LightViewMatrix);
	matLWVP = mul(matLWV, g_LightProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vPosition = mul(vPosition, matLWVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(In.vNormal, g_WorldMatrix).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vLightDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse *= g_Alpha;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_LIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_HIT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * vector(1.f, 0.f, 0.f, 1.f);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}



technique11 DefaultTechinque
{
	pass Default
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Hit
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HIT();
	}

	pass Alpha
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_LIGHT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LIGHT();
	}
}