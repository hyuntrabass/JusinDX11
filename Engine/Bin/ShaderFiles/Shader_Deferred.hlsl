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
float2 g_vCamNF;
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
Texture2D g_BlurTexture;
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
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
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
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
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
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    //float fViewRealZ = g_vCamNF.x * g_vCamNF.y / ((g_vCamNF.y - g_vCamNF.x) * vDepthDesc.x - g_vCamNF.y);
    
    vector vFogColor = vector(0.9f, 0.9f, 0.9f, 1.f);
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    
    float fNearViewZ1 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord1).y * g_vCamNF.y;
    float fNearViewZ2 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord2).y * g_vCamNF.y;
    float fNearViewZ3 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord3).y * g_vCamNF.y;
    float fNearViewZ4 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord4).y * g_vCamNF.y;
    float fNearViewZ5 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord5).y * g_vCamNF.y;
    float fNearViewZ6 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord6).y * g_vCamNF.y;
    float fNearViewZ7 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord7).y * g_vCamNF.y;
    float fNearViewZ8 = g_DepthTexture.Sample(LinearSampler, Input.vNearTexcoord8).y * g_vCamNF.y;
    
    if (abs(fViewZ - fNearViewZ1) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ2) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ3) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ4) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ5) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ6) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ7) > 0.03f * fViewZ ||
        abs(fViewZ - fNearViewZ8) > 0.03f * fViewZ)
    {
        Output.vColor = fFogFactor * (vector) 0 + (1.f - fFogFactor) * vFogColor;
        return Output;
    }
    
    //float depthDiff1 = fNearViewZ8 - fNearViewZ4;
    //float depthDiff2 = fNearViewZ6 - fNearViewZ2;
    
    //float edgeStrength = 1.f - 64.f * (depthDiff1 * depthDiff1 + depthDiff2 * depthDiff2);
    
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

    Output.vColor = fFogFactor * Output.vColor + (1.f - fFogFactor) * vFogColor /** edgeStrength*/ /** (0.1f + 0.9 * n)*/;
    
    return Output;
}

float Gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2 * sigma * sigma)) / (sigma * sqrt(2 * 3.1415926));
}

PS_OUT PS_Main_Blur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 fTexelSize = 1.f / float2(g_fScreenWidth, g_fScreenHeight);
    
    //float weight;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 1) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 2) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 3) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 4) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 5) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 6) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 7) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 8) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 9) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 10) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 11) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 12) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 13) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 14) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 15) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 16) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 17) * 1.f / 19.f;
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + fTexelSize * 18) * 1.f / 19.f;
    
    //weight = Gaussian(0, 2.0) * Gaussian(0, 2.0); // 가우시안 함수를 사용한 가중치
    //Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord) * 1.f / 19.f;
    
    for (int y = -10; y <= 10; ++y)
    {
        for (int x = -10; x <= 10; ++x)
        {
            //float weight = Gaussian(x, 2.0) * Gaussian(y, 2.0); // 가우시안 함수를 사용한 가중치
            Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + float2(x, y) * fTexelSize) * 1.f / 400.f;
        }
    }
    
    //Output.vColor = g_BlurTexture.Sample(LinearSampler, Input.vTexcoord);
    
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

    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Blur();
    }
};