matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4			g_vColor = (float4)1.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float4		vColor : COLOR0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vColor = In.vColor * g_vColor;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;

};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = In.vColor;

	return Out;
}

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

BlendState BS_None
{
	BlendEnable[0] = false;
};


technique11 DefaultTechinque
{
	pass Navigation
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}

