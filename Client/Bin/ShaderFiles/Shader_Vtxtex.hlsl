matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
    float3 vPos : Position;
    float2 vTex : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    float2 vTex : Texcoord0;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    vector vPosition = mul(float4(Input.vPos, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
	
    //Output.vPos = vPosition;
    Output.vPos = vector(Input.vPos, 1.f);
    Output.vTex = Input.vTex;
	
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
	
    //Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    Output.vColor = vector(Input.vTex.y, 0.2f, 1.f - Input.vTex.y, 1.f);

    return Output;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }
};