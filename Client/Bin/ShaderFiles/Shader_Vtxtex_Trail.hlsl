#include "Engine_Shader_Define.hlsli"

matrix g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vCamPos;

struct VS_IN
{
    float3 vPos : Position;
    float2 vPSize : PSize;
};

struct VS_OUT
{
    vector vPos : Position;
    float2 vPSize : PSize;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    Output.vPos = vector(Input.vPos, 1.f);
    Output.vPSize = Input.vPSize;
    
    return Output;
}

struct GS_IN
{
    vector vPos : Position;
    float2 vPSize : PSize;
};

struct GS_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

[maxvertexcount(12)]
void GS_Main(line GS_IN Input[2], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[6];
    
    float3 vLook = (g_vCamPos - Input[0].vPos).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * Input[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * Input[0].vPSize.y * 0.5f;
    
    float3 vNextLook = (g_vCamPos - Input[1].vPos).xyz;
    float3 vNextRight = normalize(cross(float3(0.f, 1.f, 0.f), vNextLook)) * Input[1].vPSize.x * 0.5f;
    float3 vNextUp = normalize(cross(vNextLook, vNextRight)) * Input[1].vPSize.y * 0.5f;
    
    Output[0].vPos = vector(Input[0].vPos.xyz + vRight + vUp, 1.f);
    Output[0].vTex = float2(0.f, 0.f);

    Output[1].vPos = vector(Input[0].vPos.xyz - vRight + vUp, 1.f);
    Output[1].vTex = float2(1.f, 0.f);

    Output[2].vPos = vector(Input[0].vPos.xyz - vRight - vUp, 1.f);
    Output[2].vTex = float2(1.f, 1.f);

    Output[3].vPos = vector(Input[0].vPos.xyz + vRight - vUp, 1.f);
    Output[3].vTex = float2(0.f, 1.f);
    
    Output[4].vPos = vector(Input[1].vPos.xyz + vNextRight + vNextUp, 1.f);
    Output[4].vTex = float2(0.f, 0.f);

    Output[5].vPos = vector(Input[1].vPos.xyz + vNextRight - vNextUp, 1.f);
    Output[5].vTex = float2(0.f, 1.f);

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[3].vPos = mul(Output[3].vPos, matVP);
    Output[4].vPos = mul(Output[4].vPos, matVP);
    Output[5].vPos = mul(Output[5].vPos, matVP);

    Triangles.Append(Output[0]);
    Triangles.Append(Output[1]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[2]);
    Triangles.Append(Output[3]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[1]);
    Triangles.Append(Output[4]);
    Triangles.Append(Output[5]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[1]);
    Triangles.Append(Output[5]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
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
    
    Output.vColor = vector(1.f, 0.f, 0.f, 1.f);
    
    return Output;
}

technique11 DefaultTechnique
{
    pass SingleColorTrail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }
};