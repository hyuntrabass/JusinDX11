#include "Level_Logo.h"
#include "Level_Loading.h"
#include "UI_Manager.h"

CLevel_Logo::CLevel_Logo(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOGO);

	if (FAILED(CUI_Manager::Get_Instance()->Ready_UI_Logo()))
	{
		MSG_BOX("Failed to Ready UI_Logo");
	}

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (not m_hasBGMStarted and m_pGameInstance->Is_SoundManager_Ready())
	{
		m_pGameInstance->PlayBGM(TEXT("Buzz"), 0.5f);
		m_hasBGMStarted = true;
	}

	if (CUI_Manager::Get_Instance()->Is_ButtonPushed(0))
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
	if (CUI_Manager::Get_Instance()->Is_ButtonPushed(3))
	{
		//ExitWindows(0, 0); // ÄÄÅÍ°¡ ²¨Áü;;
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Logo::Render()
{
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
