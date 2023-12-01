#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
vector g_vColor;

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
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = mul(vector(Input.vNor, 0.f), g_WorldMatrix);
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
    fShade = ceil(fShade * 2.f) / 2.f;
    
    vector vLook = Input.vWorldPos - g_vCamPos;
    //float dp = dot(normalize(vLook) * -1.f, normalize(vNormal));
    //vMtrlDiffuse = vMtrlDiffuse * dp;
    //if (dp < 0.05f)
    //{
    //    Output.vColor = vector(0.f, 0.f, 0.f, 1.f);
    //}
    //else
    //{

    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    //float fSpecular = 0.f;
    float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular);
    
    //Output.vColor = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    //}
    return Output;
}

PS_OUT PS_Main_Sky(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    return Output;
}

PS_OUT PS_Main_Single(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_vColor;
    vector vNormal = Input.vNor;
    
    vector vLook = Input.vWorldPos - g_vCamPos;
    
    float dp = dot(normalize(vLook) * -1.f, normalize(vNormal));
    
    vMtrlDiffuse = vMtrlDiffuse * dp;
    vMtrlDiffuse.a = 1.f;
     
    float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
    //fShade = ceil(fShade * 2.f) / 2.f;
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
        
    return Output;
}

PS_OUT PS_Main_Effect(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor = g_vColor;
    
    return Output;
}

PS_OUT PS_Main_MaskEffect(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor = g_vColor;
    Output.vColor.a = Output.vColor.a * vMask.r;
    
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
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass BlendMeshes
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Sky
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sky();
    }

    pass SingleColored
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Single();
    }

    pass SingleColoredEffect
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect();
    }

    pass MaskEffect
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskEffect();
    }

    pass SingleColoredEffectFrontCull
    {
        SetRasterizerState(RS_CCW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect();
    }
};