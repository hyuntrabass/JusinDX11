matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

// temp
vector g_vLightDir = vector(0.f, -1.f, 1.f, 0.f);

vector g_vLightPos = vector(20.f, 5.f, 20.f, 1.f);
float g_fRange = 150.f;

vector g_vLightDiffuse = vector(0.9f, 1.f, 0.9f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMtrlSpecular = vector(0.7f, 1.0f, 0.7f, 1.f);

vector g_vCamPos;


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

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
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
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, Input.vTex * 100.f);
    
    float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, Input.vNor));
    
    vector vReflect = reflect(normalize(g_vLightDir), Input.vNor);
    vector vLook = Input.vWorldPos - g_vCamPos;
    float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular);
    
    return Output;
}

PS_OUT PS_Main_Point(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, Input.vTex * 50.f);
    
    vector vLightDir = Input.vWorldPos - g_vLightPos;
    float fDist = length(vLightDir);
    float fAtt = pow(saturate((g_fRange - fDist) / g_fRange), 1.f);
    float fShade = saturate(dot(normalize(vLightDir) * -1.f, Input.vNor));
    
    vector vReflect = reflect(normalize(vLightDir), Input.vNor);
    vector vLook = Input.vWorldPos - g_vCamPos;
    float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 100.f);
    
    Output.vColor = ((g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt;
    
    return Output;
}

technique11 DefaultTechniqueShader_VtxNorTex
{
    pass Terrain
    {
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }
    pass Terrain_Point
    {
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Main_Point();
    }
};