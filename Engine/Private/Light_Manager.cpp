#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iLevelIndex, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return nullptr;
	}

	if (iIndex >= m_pLights[iLevelIndex].size())
	{
		return nullptr;
	}

	auto& iter = m_pLights[iLevelIndex].begin();

	for (size_t i = 0; i < iIndex; i++)
	{
		iter++;
	}

	if (iter == m_pLights[iLevelIndex].end())
	{
		return nullptr;
	}

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Init(_uint iNumLevels)
{
	if (m_pLights)
	{
		return E_FAIL;
	}

	m_pLights = new list<CLight*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return E_FAIL;
	}

	CLight* pLight = CLight::Create(LightDesc);
	if (!pLight)
	{
		return E_FAIL;
	}

	m_pLights[iLevelIndex].push_back(pLight);

	return S_OK;
}

void CLight_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return;
	}

	for (auto& pLight : m_pLights[iLevelIndex])
	{
		Safe_Release(pLight);
	}

	m_pLights[iLevelIndex].clear();
}

HRESULT CLight_Manager::Render(_uint iLevelIndex, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return E_FAIL;
	}

	for (auto& pLight : m_pLights[0])
	{
		if (FAILED(pLight->Render(pShader, pVIBuffer)))
		{
			return E_FAIL;
		}
	}

	for (auto& pLight : m_pLights[iLevelIndex])
	{
		if (FAILED(pLight->Render(pShader, pVIBuffer)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CLight_Manager* CLight_Manager::Create(_uint iNumLevels)
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Init(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& pLight : m_pLights[i])
		{
			Safe_Release(pLight);
		}
		m_pLights[i].clear();
	}
	Safe_Delete_Array(m_pLights);
}
