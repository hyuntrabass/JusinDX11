
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;

struct VS_In
{
	float3 vPos;
	float2 vTex;
};

struct VS_Out
{
	vector vPos; // == float4
	float2 vTex;
};

VS_Out VS_Main(Vs_In In)
{
	VS_Out Out;

	return Out;
}

struct PS_In
{
	vector vPos;
	float2 vTex;
};

vector PS_Main(PS_In In)
{
	vector vColor = g_Texture.Sample(, In.vTex);

	return Color
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_Main();
		PixelShader = compile ps_5_0 PS_Main();
	};
};