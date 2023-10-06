#include "MapEditorApp.h"
#include "GameInstance.h"
#include "ImguiMgr.h"

CMapEditorApp::CMapEditorApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapEditorApp::Init()
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	RECT rc{};
	GetClientRect(g_hWnd, &rc);

	GRAPHIC_DESC GraphicDesc;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iWinSizeX = rc.right - rc.left;
	GraphicDesc.iWinSizeY = rc.bottom - rc.top;
	GraphicDesc.isWindowed = TRUE;

	m_pGameInstance->Init_Engine(ToIndex(Level_ID::End), GraphicDesc, &m_pDevice, &m_pContext);

	m_pImguiMgr = CImguiMgr::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CMapEditorApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	m_fTimeAcc += fTimeDelta;

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMapEditorApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	//m_pRenderer->Draw_RenderGroup();

	if (FAILED(m_pImguiMgr->DrawEditor()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Present();

	return S_OK;
}

CMapEditorApp* CMapEditorApp::Create()
{
	CMapEditorApp* pInstance = new CMapEditorApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CMapEditorApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapEditorApp::Free()
{
	Safe_Release(m_pImguiMgr);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
