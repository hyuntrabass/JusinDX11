#include "ImguiMgr.h"
#include "GameInstance.h"
#include "Camera_Debug.h"
#include <locale>
#include <codecvt>

using namespace ImGui;

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

void CImguiMgr::Tick()
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);

	const _char* Test[3]{ "Tutorial", "Stage1", "Stage2" };
	Begin("Editor");

	if (Combo("Stage", &m_Curr_Stage, Test, IM_ARRAYSIZE(Test)))
	{
		m_DummyList.clear();
		m_pGameInstance->Open_Level(m_Curr_Stage, nullptr);
		Load_Data();
	}

	if (Button("Load", ImVec2(100.f, 50.f)))
	{
		if (FAILED(Load_Data()))
		{
			MSG_BOX("Failed to Load Map Data!");
		}
	}SameLine();
	if (Button("Export", ImVec2(100.f, 50.f)))
	{
		if (FAILED(Export_Data()))
		{
			MSG_BOX("Failed to Export Map Data!");
		}
	}

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

	static _int iCurrListIndex{};
	if (BeginTabItem("Map"))
	{
		ListBox("Misc", &iCurrListIndex, m_pItemList_Map, IM_ARRAYSIZE(m_pItemList_Map));

		m_eItemType = ItemType::Map;
		EndTabItem();
	}
	if (BeginTabItem("Misc"))
	{
		ListBox("Misc", &iCurrListIndex, m_pItemList_Misc, IM_ARRAYSIZE(m_pItemList_Misc));

		m_eItemType = ItemType::Misc;
		EndTabItem();
	}
	if (BeginTabItem("Monster"))
	{
		ListBox("Monster", &iCurrListIndex, m_pItemList_Monster, IM_ARRAYSIZE(m_pItemList_Monster));

		m_eItemType = ItemType::Monster;
		EndTabItem();
	}
	if (BeginTabItem("NPC"))
	{
		ListBox("NPC", &iCurrListIndex, m_pItemList_NPC, IM_ARRAYSIZE(m_pItemList_NPC));

		m_eItemType = ItemType::NPC;
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
	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON))
	{
		_float3 vPickPos{};
		if (m_pGameInstance->Picking_InWorld(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), &vPickPos) ||
			m_pGameInstance->Picking_InWorld(XMVectorSet(300.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), &vPickPos))
		{
			m_pPos.x = vPickPos.x;
			m_pPos.z = vPickPos.z;
		}
	}
	static _float fHorizental_Step{ 1.f };
	static _float fVertical_Step{ 0.5f };
	NewLine();
	Text("Step Amount");
	PushItemWidth(50.f);
	InputFloat("Horizental", &fHorizental_Step); SameLine();
	InputFloat("Vertical", &fVertical_Step);
	PopItemWidth();

	if (m_pGameInstance->Key_Down(DIK_UP) || m_pGameInstance->Gamepad_Down(XINPUT_UP))
	{
		m_pPos.z += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_DOWN) || m_pGameInstance->Gamepad_Down(XINPUT_DOWN))
	{
		m_pPos.z -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT) || m_pGameInstance->Gamepad_Down(XINPUT_RIGHT))
	{
		m_pPos.x += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_LEFT) || m_pGameInstance->Gamepad_Down(XINPUT_LEFT))
	{
		m_pPos.x -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RSHIFT))
	{
		m_pPos.y += fVertical_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RCONTROL))
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

	if (m_pGameInstance->Key_Pressing(DIK_V))
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
		m_pLook.x = 0.0001f;
		m_pLook.y = 1.f;
		m_pLook.z = 0.f;
		m_pLook.w = 0.f;
	} SameLine();

	if (Button("-Y", vButtonSize))
	{
		m_pLook.x = 0.0001f;
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

	if (Button("Create") || m_pGameInstance->Key_Down(DIK_C) || m_pGameInstance->Gamepad_Down(XINPUT_RB))
	{
		Create_Dummy(iCurrListIndex);
		m_pGameInstance->Gamepad_Vibration(32000, 16000);
	} SameLine();
	if (Button("Modify"))
	{

	}

	EndChild();
	End();
}

HRESULT CImguiMgr::Render()
{

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	return S_OK;
}

HRESULT CImguiMgr::Ready_Layers()
{
	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(-10.f, 15.f, -10.f, 1.f);
	CamDesc.vFocusPos = _float4(10.f, 0.f, 10.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 70000.f;

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &m_pPos)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Sky"), TEXT("Prototype_GameObject_Sky"))))
	{
		return E_FAIL;
	}

	DummyInfo Info{};
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	wstring_convert<convert_typeX, wchar_t> converterX;

	Info.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	Info.vLook = _float4(0.f, 0.f, 1.f, 0.f);
	Info.eType = ItemType::Map;
	Info.iStageIndex = 0;

	for (size_t i = 0; i < IM_ARRAYSIZE(m_pItemList_Map); i++)
	{
		Info.Prototype = L"Prototype_Model_";
		Info.Prototype += converterX.from_bytes(m_pItemList_Map[i]);
		Info.iIndex = i;
		if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

	return S_OK;
}

void CImguiMgr::Create_Dummy(const _int& iListIndex)
{
	DummyInfo Info{};
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	wstring_convert<convert_typeX, wchar_t> converterX;

	Info.vPos = m_pPos;
	XMStoreFloat4(&Info.vLook, XMVector4Normalize(XMLoadFloat4(&m_pLook)));
	Info.Prototype = L"Prototype_Model_";
	Info.eType = m_eItemType;
	switch (m_eItemType)
	{
	case MapEditor::ItemType::Map:
		Info.Prototype += converterX.from_bytes(m_pItemList_Map[iListIndex]);
		break;
	case MapEditor::ItemType::Misc:
		Info.Prototype += converterX.from_bytes(m_pItemList_Misc[iListIndex]);
		break;
	case MapEditor::ItemType::Monster:
		Info.Prototype += converterX.from_bytes(m_pItemList_Monster[iListIndex]);
		break;
	case MapEditor::ItemType::NPC:
		Info.Prototype += converterX.from_bytes(m_pItemList_NPC[iListIndex]);
		break;
	}
	Info.iIndex = iListIndex;
	Info.iStageIndex = m_Curr_Stage;

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : Dummy");
	}

	m_DummyList.push_back(Info);
}

HRESULT CImguiMgr::Load_Data()
{
	wstring strFilePath = L"../../Client/Bin/Resources/Map/Map_Data" + std::to_wstring(m_Curr_Stage) + L".hyntramap";

	ifstream InFile(strFilePath.c_str(), ios::binary);

	if (InFile.is_open())
	{
		m_DummyList.clear();

		size_t DummySize{};
		InFile.read(reinterpret_cast<_char*>(&DummySize), sizeof size_t);

		for (size_t i = 0; i < DummySize; i++)
		{
			DummyInfo Info{};
			size_t NameSize{};

			InFile.read(reinterpret_cast<_char*>(&NameSize), sizeof size_t);
			wchar_t* pBuffer = new wchar_t[NameSize / sizeof(wchar_t)];
			InFile.read(reinterpret_cast<_char*>(pBuffer), NameSize);
			Info.Prototype = pBuffer;
			Safe_Delete_Array(pBuffer);
			InFile.read(reinterpret_cast<_char*>(&Info.eType), sizeof ItemType);
			InFile.read(reinterpret_cast<_char*>(&Info.iIndex), sizeof _uint);
			InFile.read(reinterpret_cast<_char*>(&Info.iStageIndex), sizeof _uint);
			InFile.read(reinterpret_cast<_char*>(&Info.vPos), sizeof _float4);
			InFile.read(reinterpret_cast<_char*>(&Info.vLook), sizeof _float4);

			if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
			{
				MSG_BOX("Failed to Add Layer : Dummy");
			}
			else
			{
				m_DummyList.push_back(Info);
			}
		}
		InFile.close();
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CImguiMgr::Export_Data()
{
	wstring strFilePath = L"../../Client/Bin/Resources/Map/Map_Data" + std::to_wstring(m_Curr_Stage) + L".hyntramap";

	ofstream OutFile(strFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		size_t DummySize = m_DummyList.size();
		OutFile.write(reinterpret_cast<_char*>(&DummySize), sizeof size_t);

		for (auto& Data : m_DummyList)
		{
			size_t iNameSize = Data.Prototype.size() * sizeof(wchar_t) + sizeof(wchar_t);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(reinterpret_cast<const _char*>(Data.Prototype.data()), iNameSize);
			OutFile.write(reinterpret_cast<const _char*>(&Data.eType), sizeof ItemType);
			OutFile.write(reinterpret_cast<const _char*>(&Data.iIndex), sizeof _uint);
			OutFile.write(reinterpret_cast<const _char*>(&Data.iStageIndex), sizeof _uint);
			OutFile.write(reinterpret_cast<const _char*>(&Data.vPos), sizeof _float4);
			OutFile.write(reinterpret_cast<const _char*>(&Data.vLook), sizeof _float4);
		}
		OutFile.close();
	}
	else
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
