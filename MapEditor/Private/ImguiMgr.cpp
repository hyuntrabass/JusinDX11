#include "ImguiMgr.h"
#include "GameInstance.h"

CImguiMgr::CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CImguiMgr::Init()
{
	IMGUI_CHECKVERSION();
	CreateContext();

	StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CImguiMgr::DrawEditor()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

	Tick();

	Render();

	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	return S_OK;
}

void CImguiMgr::Tick()
{
	ShowDemoWindow(&m_bDemo);

	const _char* Test[3]{ "Tutorial", "Stage1", "Stage2" };
	Begin("Editor");

	Combo("Stage", &m_Curr_Stage, Test, IM_ARRAYSIZE(Test));

	Button("Load", ImVec2(100.f, 50.f)); SameLine();
	Button("Export", ImVec2(100.f, 50.f));

	switch (m_Curr_Stage)
	{
	case 0:
		BeginChild("Tutorial Stage");
		break;
	case 1:
		BeginChild("Stage1");
		break;
	case 2:
		BeginChild("Stage2");
		break;
	default:
		break;
	}

	BeginTabBar("Tab bar");

	if (BeginTabItem("Misc"))
	{
		ListBox("Misc", &m_Curr_Misc, m_pItemList_Misc, IM_ARRAYSIZE(m_pItemList_Misc));

		EndTabItem();
	}
	if (BeginTabItem("Monster"))
	{
		ListBox("Monster", &m_Curr_Monster, m_pItemList_Monster, IM_ARRAYSIZE(m_pItemList_Monster));

		EndTabItem();
	}
	if (BeginTabItem("NPC"))
	{
		ListBox("NPC", &m_Curr_NPC, m_pItemList_NPC, IM_ARRAYSIZE(m_pItemList_NPC));

		EndTabItem();
	}
	EndTabBar();

	InputFloat3("Pos", m_pPos, "%.2f"); SameLine(); 
	if (Button("reset Pos"))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(m_pPos); i++)
		{
			m_pPos[i] = 0.f;
		}
	}

	InputFloat3("Look", m_pLook, "%.2f"); SameLine(); 
	if (Button("reset Look"))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(m_pLook); i++)
		{
			m_pLook[i] = 0.f;
		}
	}

	EndChild();
	End();
}

CImguiMgr* CImguiMgr::Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance)
{
	CImguiMgr* pInstance = new CImguiMgr(pDevice, pContext, pGameInstance);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CImguiMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImguiMgr::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();

}
