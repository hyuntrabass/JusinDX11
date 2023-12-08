#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

LIGHT_DESC* CLight::Get_LightDesc()
{
	return &m_LightDesc;
}

HRESULT CLight::Init(const LIGHT_DESC& LightDeesc)
{
	m_LightDesc = LightDeesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint iPassIndex{};

	if (m_LightDesc.eType == LIGHT_DESC::Directional)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof _float4)))
		{
			return E_FAIL;
		}
		iPassIndex = DefPass_Light_Directional;
	}
	else if (m_LightDesc.eType == LIGHT_DESC::Point)
	{
		iPassIndex = DefPass_Light_Point;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof _float4)))
		{
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightAtt", &m_LightDesc.vAttenuation, sizeof _float4)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Begin(iPassIndex)))
	{
		return E_FAIL;
	}

	if (FAILED(pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Init(LightDesc)))
	{
		MSG_BOX("Failed to Create : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}
