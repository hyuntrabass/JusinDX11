#include "Light.h"

CLight::CLight()
{
}

const LIGHT_DESC* CLight::Get_LightDesc() const
{
	return &m_LightDesc;
}

HRESULT CLight::Init(const LIGHT_DESC& LightDeesc)
{
	m_LightDesc = LightDeesc;

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
