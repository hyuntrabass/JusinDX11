#include "Engine_Shader_Define.hlsli"

matrix g_BoneMatrices[256];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

vector g_vLightDir;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMtrlSpecular = vector(0.8f, 0.8f, 0.8f, 1.f);

vector g_vCamPos;

float g_fNorTex;
bool g_bSelected = false;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
    uint4 vBlendIndices : BlendIndex;
    float4 vBlendWeight : BlendWeight;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    float fW = 1.f - (Input.vBlendWeight.x + Input.vBlendWeight.y + Input.vBlendWeight.z);
    matrix Bone = g_BoneMatrices[Input.vBlendIndices.x] * Input.vBlendWeight.x +
    g_BoneMatrices[Input.vBlendIndices.y] * Input.vBlendWeight.y +
    g_BoneMatrices[Input.vBlendIndices.z] * Input.vBlendWeight.z +
    g_BoneMatrices[Input.vBlendIndices.w] * fW;
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    vector vNor = mul(vector(Input.vNor, 0.f), Bone);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNor, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
	
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + 1.f * g_bSelected;
    vector vNormal = (g_NormalTexture.Sample(LinearSampler, Input.vTex) * 2 - 1) * g_fNorTex + Input.vNor;
    
    float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    vector vLook = Input.vWorldPos - g_vCamPos;
    //float fSpecular = 0.f;
    float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular);
    
    //Output.vColor = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    return Output;
}

PS_OUT PS_Main_Test(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.7f, 0.7f, 0.7f, 1.f);
    vector vNormal = Input.vNor;
    
    float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
    
    return Output;
}

technique11 DefaultTechniqueShader_VtxNorTex
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Test
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main_Test();
    }
};