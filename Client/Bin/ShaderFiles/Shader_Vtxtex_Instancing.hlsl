#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

struct VS_IN
{
    float3 vPos : Position;
    float2 vTex : Texcoord0;
    
    float4 vRight : Texcoord1;
    float4 vUp : Texcoord2;
    float4 vLook : Texcoord3;
    float4 vTranslation : Texcoord4;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    float2 vTex : Texcoord0;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix Transform = matrix(Input.vRight, Input.vUp, Input.vLook, Input.vTranslation);
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(Input.vPos, 1.f), Transform);
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vPosition = mul(vPosition, matWVP);
    
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
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Particle
    {
        SetRasterizerState(RS_CCW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }
};