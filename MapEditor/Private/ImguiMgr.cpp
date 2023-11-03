#include "ImguiMgr.h"
#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Dummy.h"

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

void CImguiMgr::SetPos(const _float4& vPos, CDummy* pDummy)
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CameraPos());
	_float fNewDist = XMVector4Length(vCamPos - XMLoadFloat4(&vPos)).m128_f32[0];
	if (m_fCamDist < 0 || m_fCamDist > fNewDist)
	{
		m_vPos.x = vPos.x;
		m_vPos.y = vPos.y;
		m_vPos.z = vPos.z;
		m_vPos.w = 1.f;

		m_fCamDist = fNewDist;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = pDummy;
		m_pSelectedDummy->Select(true);
	}
}

_bool CImguiMgr::ComputePickPos()
{
	return m_ComputePickPos;
}

HRESULT CImguiMgr::Init(vector<wstring>* Models, vector<string>* pPropCount)
{
	IMGUI_CHECKVERSION();
	CreateContext();

	StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_pMapModels = Models;

	if (FAILED(Ready_Layers(pPropCount)))
	{
		return E_FAIL;
	}
	Load_Data();

	return S_OK;
}

void CImguiMgr::Tick()
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);

	const _char* szStages[3]{ "Tutorial", "Stage1", "Stage2" };
	Begin("Editor");

	if (Combo("Stage", &m_Curr_Stage, szStages, IM_ARRAYSIZE(szStages)))
	{
		m_DummyList.clear();
		m_pGameInstance->Set_CurrentLevelIndex(m_Curr_Stage);
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
	if (BeginTabItem("Props"))
	{
		PushItemWidth(300.f);
		ListBox("Props", &iCurrListIndex, m_pItemList_Props, m_iNumProps);
		m_eItemType = ItemType::Props;
		EndTabItem();
		PopItemWidth();
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
	InputFloat3("Pos", (_float*)&m_vPos, "%.2f"); SameLine();
	if (Button("Reset"))
	{
		m_vPos.x = 0.f;
		m_vPos.y = 0.f;
		m_vPos.z = 0.f;
		m_vPos.w = 1.f;
	}
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
	{
		m_ComputePickPos = true;
		m_fCamDist = -1.f;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = nullptr;
		//_float3 vPickPos{};
		//if (m_pGameInstance->Picking_InWorld(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), &vPickPos) ||
		//	m_pGameInstance->Picking_InWorld(XMVectorSet(300.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), &vPickPos))
		//{
		//	m_vPos.x = vPickPos.x;
		//	m_vPos.z = vPickPos.z;
		//}
	}
	else
	{
		m_ComputePickPos = false;
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
		m_vPos.z += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_DOWN) || m_pGameInstance->Gamepad_Down(XINPUT_DOWN))
	{
		m_vPos.z -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT) || m_pGameInstance->Gamepad_Down(XINPUT_RIGHT))
	{
		m_vPos.x += fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_LEFT) || m_pGameInstance->Gamepad_Down(XINPUT_LEFT))
	{
		m_vPos.x -= fHorizental_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RSHIFT))
	{
		m_vPos.y += fVertical_Step;
	}
	if (m_pGameInstance->Key_Down(DIK_RCONTROL))
	{
		m_vPos.y -= fVertical_Step;
	}

	SeparatorText("Look");

	Tip("Press 'V' to Set Look from Marker to Cursor. Normalizes Automatically");
	InputFloat3("Look", (_float*)&m_vLook, "%.2f"); SameLine();
	if (Button("Normalize"))
	{
		XMStoreFloat4(&m_vLook, XMVector4Normalize(XMLoadFloat4(&m_vLook)));
	}

	if (m_pGameInstance->Key_Pressing(DIK_V))
	{
		_float3 vPickPos{};
		if (m_pGameInstance->Picking_InWorld(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), &vPickPos) ||
			m_pGameInstance->Picking_InWorld(XMVectorSet(300.f, 0.f, 300.f, 1.f), XMVectorSet(300.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 300.f, 1.f), &vPickPos))
		{
			m_vLook.x = vPickPos.x - m_vPos.x;
			m_vLook.z = vPickPos.z - m_vPos.z;

			XMStoreFloat4(&m_vLook, XMVector4Normalize(XMLoadFloat4(&m_vLook)));
		}
	}
	NewLine();
	ImVec2 vButtonSize = ImVec2(45.f, 30.f);
	if (Button("+X", vButtonSize))
	{
		m_vLook.x = 1.f;
		m_vLook.y = 0.f;
		m_vLook.z = 0.f;
		m_vLook.w = 0.f;
	} SameLine();

	if (Button("-X", vButtonSize))
	{
		m_vLook.x = -1.f;
		m_vLook.y = 0.f;
		m_vLook.z = 0.f;
		m_vLook.w = 0.f;
	} SameLine();

	if (Button("+Y", vButtonSize))
	{
		m_vLook.x = 0.0001f;
		m_vLook.y = 1.f;
		m_vLook.z = 0.f;
		m_vLook.w = 0.f;
	} SameLine();

	if (Button("-Y", vButtonSize))
	{
		m_vLook.x = 0.0001f;
		m_vLook.y = -1.f;
		m_vLook.z = 0.f;
		m_vLook.w = 0.f;
	} SameLine();

	if (Button("+Z", vButtonSize))
	{
		m_vLook.x = 0.f;
		m_vLook.y = 0.f;
		m_vLook.z = 1.f;
		m_vLook.w = 0.f;
	} SameLine();

	if (Button("-Z", vButtonSize))
	{
		m_vLook.x = 0.f;
		m_vLook.y = 0.f;
		m_vLook.z = -1.f;
		m_vLook.w = 0.f;
	}

	SeparatorText("");

	if (Button("Create") || m_pGameInstance->Key_Down(DIK_C) || m_pGameInstance->Gamepad_Down(XINPUT_RB))
	{
		Create_Dummy(iCurrListIndex);
		m_pGameInstance->Gamepad_Vibration(32000, 16000);
	} SameLine();
	if (Button("Modify") || m_pGameInstance->Key_Down(DIK_R))
	{
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}

	} SameLine();
	if (Button("Delete") || m_pGameInstance->Key_Down(DIK_F))
	{
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Kill();
		}
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

HRESULT CImguiMgr::Ready_Layers(vector<string>* pPropCount)
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

	//if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &m_vPos)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Sky"), TEXT("Prototype_GameObject_Sky"))))
	{
		return E_FAIL;
	}

	m_iNumProps = static_cast<int>(pPropCount->size());
	m_pItemList_Props = new const _char * [m_iNumProps];

	for (size_t i = 0; i < m_iNumProps; i++)
	{
		m_pItemList_Props[i] = (*pPropCount)[i].c_str();
	}

	return S_OK;
}

void CImguiMgr::Create_Dummy(const _int& iListIndex)
{
	DummyInfo Info{};

	Info.pImguiMgr = this;
	Info.vPos = m_vPos;
	XMStoreFloat4(&Info.vLook, XMVector4Normalize(XMLoadFloat4(&m_vLook)));
	Info.Prototype = L"Prototype_Model_";
	Info.eType = m_eItemType;
	_tchar strUnicode[MAX_PATH]{};
	switch (m_eItemType)
	{
	case MapEditor::ItemType::Props:
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Props[iListIndex], static_cast<int>(strlen(m_pItemList_Props[iListIndex])), strUnicode, static_cast<int>(strlen(m_pItemList_Props[iListIndex])));
		break;
	case MapEditor::ItemType::Monster:
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Monster[iListIndex], static_cast<int>(strlen(m_pItemList_Monster[iListIndex])), strUnicode, static_cast<int>(strlen(m_pItemList_Monster[iListIndex])));
		break;
	}
	Info.Prototype += strUnicode;
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
	DummyInfo Info{};
	Info.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	Info.vLook = _float4(0.f, 0.f, 1.f, 0.f);
	Info.eType = ItemType::Map;
	Info.iStageIndex = m_Curr_Stage;
	Info.pImguiMgr = this;
	_int i{};

	for (auto& strFileName : m_pMapModels[m_Curr_Stage])
	{
		Info.Prototype = L"Prototype_Model_";
		Info.Prototype += strFileName;
		Info.iIndex = i++;
		if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

	filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/" + std::to_wstring(m_Curr_Stage) + L".hyuntramap";

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
			Info.pImguiMgr = this;

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
	filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/" + std::to_wstring(m_Curr_Stage) + L".hyuntramap";

	ofstream OutFile(strFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		size_t DummySize = m_DummyList.size();
		OutFile.write(reinterpret_cast<_char*>(&DummySize), sizeof size_t);

		for (auto& Data : m_DummyList)
		{
			size_t iNameSize = Data.Prototype.size() * sizeof(_tchar) + sizeof(_tchar);
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

CImguiMgr* CImguiMgr::Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance, vector<wstring>* Models, vector<string>* pPropCount)
{
	CImguiMgr* pInstance = new CImguiMgr(pDevice, pContext, pGameInstance);

	if (FAILED(pInstance->Init(Models, pPropCount)))
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
