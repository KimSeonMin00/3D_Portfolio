// float4x4
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


texture2D		g_SourDiffTexture;
texture2D		g_DestDiffTexture;
texture2D		g_FilterTexture;

sampler		DefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC 

	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;

	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};


sampler		PointSampler = sampler_state
{
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal: NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal: NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition, 0.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_TERRAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vSourDiffuse = g_SourDiffTexture.Sample(DefaultSampler, In.vTexUV * 30.f);

	vector		vMtrlDiffuse = vSourDiffuse;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), In.vNormal);

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}

PS_OUT PS_MAIN_TERRAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_SourDiffTexture.Sample(DefaultSampler, In.vTexUV * 30.f);

	vector		vLightDir = In.vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fRange - fDistance), 0.f) / g_fRange;

	float		fShade = max(dot(normalize(vLightDir) * -1.f, In.vNormal), 0.f) * fAtt;

	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(vLightDir), In.vNormal);

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}



technique11 DefaultTechinque
{
	pass Terrain_Directional
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_DIRECTIONAL();
	}

	pass Terrain_Point
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_POINT();
	}

}