matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_TextureArray[12];
int g_TexIndex;

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

RasterizerState RS_Test
{
    CullMode = None;
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
	
    Output.vPos = vPosition;
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
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    if (Output.vColor.a < 0.7f)
    {
        discard;
    }

    return Output;
}

PS_OUT PS_Main_Button(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vHighlight = g_TextureArray[1].Sample(LinearSampler, Input.vTex) * g_TexIndex;
    vHighlight.a = 0.f;
    
    Output.vColor = g_TextureArray[0].Sample(LinearSampler, Input.vTex) + vHighlight;
    
    if (Output.vColor.a < 0.7f)
    {
        discard;
    }

    return Output;
}

technique11 DefaultTechnique
{
    pass UI
    {
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Button
    {
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main_Button();
    }
};