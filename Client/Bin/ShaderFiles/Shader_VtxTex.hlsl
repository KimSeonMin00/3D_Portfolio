#include "Shader_Defines.hpp"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;

float			g_Alpha = 1.f;
float			g_Ratio = 1.f;

vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = g_vColor * vMtrlDiffuse;
	Out.vColor.a = Out.vColor.a * g_Alpha;

	return Out;
}

PS_OUT PS_MAIN_RECT_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector    vRed = vector (1.f, 0.f, 0.f, 1.f);

	Out.vColor = vRed * vMtrlDiffuse;

	return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = g_vColor * vMtrlDiffuse;
	Out.vColor.a = vMtrlDiffuse.x * g_Alpha;

	return Out;
}

PS_OUT PS_MAIN_RATIO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = g_vColor * vMtrlDiffuse;
	Out.vColor.a = vMtrlDiffuse.x * g_Alpha;

	if (g_Ratio < In.vTexUV.x)
		discard;

	return Out;
}

technique11 DefaultTechinque
{
	pass Rect
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Rect_Color
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Rect_Alpha
	{
		SetBlendState(BS_One, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Rect_Alpha_BLACK
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass Rect_Ratio
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RATIO();
	}
}
