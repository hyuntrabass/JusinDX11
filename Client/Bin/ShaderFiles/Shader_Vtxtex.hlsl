matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;

struct VS_IN
{
	float3 vPos;
	float2 vTex;
};

struct VS_OUT
{
	vector vPos; // == float4
	float2 vTex;
};

VS_OUT VS_Main(VS_IN Input)
{
	VS_OUT Output;
	
	return Output;
}

struct PS_IN
{
	vector vPos;
	float2 vTex;
};

vector PS_Main(PS_IN Input)
{
	vector vColor = g_Texture.Sample(, Input.vTex);

    return vColor;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_Main();
		PixelShader = compile ps_5_0 PS_Main();
	}
};