#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightPos;
float			g_fRange = 0.f;

vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlDiffuse;
vector			g_vMtrlAmbient = vector(0.2f, 0.2f, 0.2f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vCamPosition;
vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);


texture2D		g_DiffuseTexture;
texture2D		g_AlphaTexture;

float2			g_vMoveTex = float2(0.f, 0.f);
float			g_Alpha = 1.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float		fShade = max(dot(normalize(g_vLightDir.xyz) * -1.f, In.vNormal), 0.f);

	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), vector(In.vNormal, 0.f));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor.xyz = vMtrlDiffuse.xyz;

	Out.vColor.a = vMtrlDiffuse.a;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_RECT_SELECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float		fShade = max(dot(normalize(g_vLightDir.xyz) * -1.f, In.vNormal), 0.f);

	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), vector(In.vNormal, 0.f));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor.xyz = ((g_vColor * g_vLightDiffuse * vMtrlDiffuse) * (fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular).xyz;

	Out.vColor.a = vMtrlDiffuse.a;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTexUVMove = In.vTexUV + g_vMoveTex;

	vector		vMtrlAlpha = g_AlphaTexture.Sample(DefaultSampler, vTexUVMove);

	Out.vColor = g_vColor * vMtrlAlpha;

	Out.vColor.a =  vMtrlAlpha.a * g_Alpha;


	return Out;
}

PS_OUT PS_MAIN_MOVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float		fAlpha = In.vTexUV.x;
	float2 vTexUVMove = In.vTexUV + g_vMoveTex;

	vector		vMtrlAlpha = g_AlphaTexture.Sample(DefaultSampler, vTexUVMove);

	Out.vColor = g_vColor * vMtrlAlpha;

	Out.vColor.a = vMtrlAlpha.a * g_Alpha * fAlpha * fAlpha;

	return Out;
}

technique11 DefaultTechinque
{
	pass Default
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Select
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_SELECT();
	}

	pass Effect
	{
		SetBlendState(BS_One, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass MoveTexture
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOVE();
	}

}