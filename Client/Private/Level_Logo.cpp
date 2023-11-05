#include "Level_Logo.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOGO);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Logo(TEXT("Layer_Logo"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Buttons(TEXT("Layer_MainTitle_Buttons"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CREATECHARACTER))))
		{
			MSG_BOX("Failed to Open Level");
		}
		else
		{
			return;
		}
	}
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Logo(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_Logo"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_SubTitle"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Buttons(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_StartBtn"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(_dev pDevice, _context pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
