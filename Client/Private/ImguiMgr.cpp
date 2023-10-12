#include "ImguiMgr.h"

CImguiMgr::CImguiMgr(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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
	bool bDemo{ true };
	ShowDemoWindow(&bDemo);

	const _char* Test[4]{ "Test1", "Test2", "Test3", "Test4" };
	int a = 1;
	Begin("Map Editor");
	
	Combo("Choose Level", &a, Test, IM_ARRAYSIZE(Test));

	Button("Load", ImVec2(100.f, 50.f)); SameLine();
	Button("Save", ImVec2(100.f, 50.f));

	End();
}

CImguiMgr* CImguiMgr::Create(_dev pDevice, _context pContext)
{
	CImguiMgr* pInstance = new CImguiMgr(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CImguiMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImguiMgr::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();

}
