#include "Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_CreateCharacter.h"
#include "Level_Tutorial.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"


CLevel_Loading::CLevel_Loading(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Init(Level_ID eNextLevel)
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround_Loading"))))
	{
		return E_FAIL;
	}

	//if (FAILED(Ready_Layer_LoadingBar(TEXT("Layer_Bar_Loading"))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Icon(TEXT("Layer_Icon_Loading"))))
	{
		return E_FAIL;
	}

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (!m_pLoader)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	m_pLoader->Show_LoadingText();

	if (m_pLoader->isFinished()/* && m_pGameInstance->Key_Down(VK_SPACE)*/)
	{
		CLevel* pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case Client::Level_ID::Logo:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case Client::Level_ID::CreateCharacter:
			pLevel = CLevel_CreateCharacter::Create(m_pDevice, m_pContext);
			break;
		case Client::Level_ID::Tutorial:
			pLevel = CLevel_Tutorial::Create(m_pDevice, m_pContext);
			break;
		case Client::Level_ID::Stage1:
			pLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
			break;
		case Client::Level_ID::Stage2:
			pLevel = CLevel_Stage2::Create(m_pDevice, m_pContext);
			break;
		}
		
		if (!pLevel)
		{
			return;
		}

		if (FAILED(m_pGameInstance->Open_Level(ToIndex(m_eNextLevel), pLevel)))
		{
			MSG_BOX("Failed to Open Level");
		}
		else
		{
			return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Loading), strLayerTag, TEXT("Prototype_GameObject_Loading_Screen"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingBar(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Loading), strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_Icon(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Loading), strLayerTag, TEXT("Prototype_GameObject_Loading_Icon"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(_dev pDevice, _context pContext, Level_ID eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
