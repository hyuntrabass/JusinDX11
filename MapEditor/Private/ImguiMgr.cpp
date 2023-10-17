#include "ImguiMgr.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Camera_Debug.h"

static void Tip(const char* desc)
{
	ImGui::TextDisabled("(Tip)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

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

	if (FAILED(Ready_Layers()))
	{
		return E_FAIL;
	}

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
	NewLine();

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

	SeparatorText("Position");

	Tip("Right Click to Set Position");
	InputFloat3("Pos", (_float*)&m_pPos, "%.2f"); SameLine();
	if (Button("Reset"))
	{
		m_pPos.x = 0.f;
		m_pPos.y = 0.f;
		m_pPos.z = 0.f;
		m_pPos.w = 1.f;
	}
	if (m_pGameInstance->Key_Pressing(VK_RBUTTON))
	{
		_float3 vPickPos{};
		if (m_pGameInstance->Picking_InWorld(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), &vPickPos) ||
			m_pGameInstance->Picking_InWorld(XMVectorSet(300.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), &vPickPos))
		{
			m_pPos.x = vPickPos.x;
			m_pPos.z = vPickPos.z;
		}
	}
	static _float fHorizental_Step{1.f};
	static _float fVertical_Step{0.5f};
	NewLine();
	Text("Step Amount");
	PushItemWidth(50.f);
	InputFloat("Horizental", &fHorizental_Step); SameLine();
	InputFloat("Vertical", &fVertical_Step);
	PopItemWidth();
	if (m_pGameInstance->Key_Down(VK_UP))
	{
		m_pPos.z += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(VK_DOWN))
	{
		m_pPos.z -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(VK_RIGHT))
	{
		m_pPos.x += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(VK_LEFT))
	{
		m_pPos.x -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(VK_RSHIFT))
	{
		m_pPos.y += fVertical_Step;
	}
	if (m_pGameInstance->Key_Down(VK_RCONTROL))
	{
		m_pPos.y -= fVertical_Step;
	}

	SeparatorText("Look");

	Tip("Press 'V' to Set Look from Marker to Cursor. Normalizes Automatically");
	InputFloat3("Look", (_float*)&m_pLook, "%.2f"); SameLine();
	if (Button("Normalize"))
	{
		XMStoreFloat4(&m_pLook, XMVector4Normalize(XMLoadFloat4(&m_pLook)));
	}

	if (m_pGameInstance->Key_Pressing('V'))
	{
		_float3 vPickPos{};
		if (m_pGameInstance->Picking_InWorld(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), &vPickPos) ||
			m_pGameInstance->Picking_InWorld(XMVectorSet(300.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), &vPickPos))
		{
			m_pLook.x = vPickPos.x - m_pPos.x;
			m_pLook.z = vPickPos.z - m_pPos.z;

			XMStoreFloat4(&m_pLook, XMVector4Normalize(XMLoadFloat4(&m_pLook)));
		}
	}
	NewLine();
	ImVec2 vButtonSize = ImVec2(45.f, 30.f);
	if (Button("+X", vButtonSize))
	{
		m_pLook.x = 1.f;
		m_pLook.y = 0.f;
		m_pLook.z = 0.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("-X", vButtonSize))
	{
		m_pLook.x = -1.f;
		m_pLook.y = 0.f;
		m_pLook.z = 0.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("+Y", vButtonSize))
	{
		m_pLook.x = 0.f;
		m_pLook.y = 1.f;
		m_pLook.z = 0.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("-Y", vButtonSize))
	{
		m_pLook.x = 0.f;
		m_pLook.y = -1.f;
		m_pLook.z = 0.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("+Z", vButtonSize))
	{
		m_pLook.x = 0.f;
		m_pLook.y = 0.f;
		m_pLook.z = 1.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("-Z", vButtonSize))
	{
		m_pLook.x = 0.f;
		m_pLook.y = 0.f;
		m_pLook.z = -1.f;
		m_pLook.w = 0.f;
	}

	SeparatorText("");

	if (Button("Create") || m_pGameInstance->Key_Down('C'))
	{
		DummyInfo Info{};

		Info.vPos = m_pPos;
		XMStoreFloat4(&Info.vLook, XMVector4Normalize(XMLoadFloat4(&m_pLook)));

		if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

	EndChild();
	End();
}

HRESULT CImguiMgr::Ready_Layers()
{
	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(-10.f, 15.f, -10.f, 1.f);
	CamDesc.vFocusPos = _float4(10.f, 0.f, 10.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &m_pPos)))
	{
		return E_FAIL;
	}


	return S_OK;
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
