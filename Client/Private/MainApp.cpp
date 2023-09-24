#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Init()
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	GRAPHIC_DESC GraphicDesc;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.isWindowed = TRUE;

	m_pGameInstance->Init_Engine(ToIndex(Level_ID::End), GraphicDesc, &m_pDevice, &m_pContext);
	
	if (FAILED(Open_Level(Level_ID::Logo)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	m_fTimeAcc += fTimeDelta;

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(Level_ID eLevelID)
{
	if (eLevelID == Level_ID::Loading)
	{
		return E_FAIL;
	}

	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID);
	if (!pLevel)
	{
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), pLevel);
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
