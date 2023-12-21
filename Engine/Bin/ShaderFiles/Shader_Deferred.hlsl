#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightAtt;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vCamPosition;
float g_fCamFar;
float g_fLightFar;
float g_fScreenWidth;
float g_fScreenHeight;

float2 g_vFogNF;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ShadeTexture;
Texture2D g_SpecularTexture;
Texture2D g_DepthTexture;
Texture2D g_LightDepthTexture;
Texture2D g_Texture;

struct VS_IN
{
    float3 vPos : Position;
    float2 vTexcoord : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
    float2 vNearTexcoord1 : Texcoord1;
    float2 vNearTexcoord2 : Texcoord2;
    float2 vNearTexcoord3 : Texcoord3;
    float2 vNearTexcoord4 : Texcoord4;
    float2 vNearTexcoord5 : Texcoord5;
    float2 vNearTexcoord6 : Texcoord6;
    float2 vNearTexcoord7 : Texcoord7;
    float2 vNearTexcoord8 : Texcoord8;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vTexcoord = Input.vTexcoord;
    
    float2 fTexelSize = float2(1.f / g_fScreenWidth, 1.f / g_fScreenHeight);
    
    Output.vNearTexcoord4 = Input.vTexcoord + float2(0.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord6 = Input.vTexcoord + float2(fTexelSize.x * +1.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord2 = Input.vTexcoord + float2(fTexelSize.x * +1.f, 0.f);
    Output.vNearTexcoord5 = Input.vTexcoord + float2(fTexelSize.x * +1.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord3 = Input.vTexcoord + float2(0.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord8 = Input.vTexcoord + float2(fTexelSize.x * -1.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord7 = Input.vTexcoord + float2(fTexelSize.x * -1.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord1 = Input.vTexcoord + float2(fTexelSize.x * -1.f, 0.f);
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
    float2 vNearTexcoord1 : Texcoord1;
    float2 vNearTexcoord2 : Texcoord2;
    float2 vNearTexcoord3 : Texcoord3;
    float2 vNearTexcoord4 : Texcoord4;
    float2 vNearTexcoord5 : Texcoord5;
    float2 vNearTexcoord6 : Texcoord6;
    float2 vNearTexcoord7 : Texcoord7;
    float2 vNearTexcoord8 : Texcoord8;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main_Debug(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTexcoord);
    
    return Output;
}

struct PS_OUT_Light
{
    vector vShade : SV_Target0;
    vector vSpecular : SV_Target1;
};

PS_OUT_Light PS_Main_Directional(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, Input.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    //Output.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    Output.vShade = g_vLightDiffuse * saturate(ceil(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient); // 카툰
    //Output.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient); // 카툰X
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos;
    
    // 투영 스페이스로
    vWorldPos.x = Input.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = Input.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    // w 나누기 제거
    vWorldPos = vWorldPos * fViewZ;
    
    // 뷰스페이스로
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    // 월드로
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);
    
    return Output;
}

PS_OUT_Light PS_Main_Point(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, Input.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    vector vWorldPos;
    
    // 투영 스페이스로
    vWorldPos.x = Input.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = Input.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    // w 나누기 제거
    vWorldPos = vWorldPos * fViewZ;
    
    // 뷰스페이스로
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    // 월드로
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = 1.f / (g_vLightAtt.y + g_vLightAtt.z * fDistance + g_vLightAtt.w * (fDistance * fDistance));

    //Output.vShade = fAtt * (g_vLightDiffuse * saturate(ceil(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient));
    Output.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient));

    vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = fAtt * (g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f));
    
    return Output;
}

PS_OUT PS_Main_Deferred(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTexcoord);
    if (vDiffuse.a == 0.f)
    {
        discard;
    }
    vector vShade = g_ShadeTexture.Sample(LinearSampler, Input.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    
    Output.vColor = vDiffuse * vShade + vSpecular;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vFogColor = vector(0.9f, 0.9f, 0.9f, 1.f);
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    
    float fNearViewZ1 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord1).y * g_fCamFar;
    float fNearViewZ2 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord2).y * g_fCamFar;
    float fNearViewZ3 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord3).y * g_fCamFar;
    float fNearViewZ4 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord4).y * g_fCamFar;
    float fNearViewZ5 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord5).y * g_fCamFar;
    float fNearViewZ6 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord6).y * g_fCamFar;
    float fNearViewZ7 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord7).y * g_fCamFar;
    float fNearViewZ8 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord8).y * g_fCamFar;
    
    if (abs(fViewZ - fNearViewZ1) > 1.f ||
        abs(fViewZ - fNearViewZ2) > 1.f ||
        abs(fViewZ - fNearViewZ3) > 1.f ||
        abs(fViewZ - fNearViewZ4) > 1.f ||
        abs(fViewZ - fNearViewZ5) > 1.f ||
        abs(fViewZ - fNearViewZ6) > 1.f ||
        abs(fViewZ - fNearViewZ7) > 1.f ||
        abs(fViewZ - fNearViewZ8) > 1.f)
    {
        Output.vColor = fFogFactor * (vector) 0 + (1.f - fFogFactor) * vFogColor;
        return Output;
    }

    //float3 vCenter = g_NormalTexture.Sample(LinearSampler, Input.vTexcoord).xyz;
    //float3 vT = g_NormalTexture.Sample(LinearSampler, Input.vNearTexcoord1).xyz;
    //float3 vTR = g_NormalTexture.Sample(LinearSampler, Input.vNearTexcoord2).xyz;
    //float3 vR = g_NormalTexture.Sample(LinearSampler, Input.vNearTexcoord3).xyz;
    
    //vT = abs(vCenter - vT);
    //vTR = abs(vCenter - vTR);
    //vR = abs(vCenter - vR);
    
    //float n = 0;
    //n = max(n, vT.x);
    //n = max(n, vT.y);
    //n = max(n, vT.z);
    //n = max(n, vR.x);
    //n = max(n, vR.y);
    //n = max(n, vR.z);
    //n = max(n, vTR.x);
    //n = max(n, vTR.y);
    //n = max(n, vTR.z);
    
    //n = 1.f - clamp(clamp((n * 2.f) - 0.8f, 0.f, 1.f) * 1.5f, 0.f, 1.f);
    
    float4 vWorldPos;
    
    vWorldPos.x = Input.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = Input.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);
    
    float2 vLightDepthUV;
    vLightDepthUV.x = vWorldPos.x / vWorldPos.w * 0.5f + 0.5f;
    vLightDepthUV.y = vWorldPos.y / vWorldPos.w * -0.5f + 0.5f;
    
    vector vLightDepthDesc = g_LightDepthTexture.Sample(LinearClampSampler, vLightDepthUV);
    
    float fLightDepth = vLightDepthDesc.x * g_fLightFar;
    
    if (vWorldPos.w - 0.001f > fLightDepth)
    {
        Output.vColor.xyz = Output.vColor.xyz * 0.5f;
    }

    Output.vColor = fFogFactor * Output.vColor + (1.f - fFogFactor) * vFogColor /** (0.1f + 0.9 * n)*/;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Debug();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Directional();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Point();
    }

    pass Deferrd
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Deferred();
    }
};