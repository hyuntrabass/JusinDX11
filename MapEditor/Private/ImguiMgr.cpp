#include "ImguiMgr.h"
#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Dummy.h"
#include "3D_Cursor.h"

IMPLEMENT_SINGLETON(CImguiMgr)

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

CImguiMgr::CImguiMgr()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

const _float4& CImguiMgr::Get_Position() const
{
	return m_vPos;
}

const _bool& CImguiMgr::HasTo_PlayeScene() const
{
	return m_PlayScene;
}

CUTSCENE* CImguiMgr::Get_CurrentScene()
{
	return m_pCurrentScene;
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
	}
}

void CImguiMgr::Select(const _float4& vPos, CDummy* pDummy)
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CameraPos());
	_float fNewDist = XMVector4Length(vCamPos - XMLoadFloat4(&vPos)).m128_f32[0];
	if (m_fCamDist < 0 || m_fCamDist > fNewDist)
	{
		m_fCamDist = fNewDist;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = pDummy;
		m_pSelectedDummy->Select(true);

		m_pSelectedDummy->Get_State(m_vPos, m_vLook);
	}
}

void CImguiMgr::End_Scene()
{
	m_PlayScene = false;
}

_bool CImguiMgr::ComputePickPos()
{
	return m_ComputePickPos;
}

_bool CImguiMgr::ComputeSelection()
{
	return m_ComputeSelection;
}

HRESULT CImguiMgr::Init(_dev pDevice, _context pContext, vector<wstring>* MapList, vector<wstring>* MapCOLList, vector<string>* PropList)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

	IMGUI_CHECKVERSION();
	CreateContext();

	StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_pMapModels = MapList;
	m_pMapCOLModels = MapCOLList;

	if (FAILED(Ready_Layers(PropList)))
	{
		return E_FAIL;
	}
	Load_Data();

	return S_OK;
}

void CImguiMgr::Tick(_float fTimeDelta)
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);

	const _char* szStages[3]{ "Tutorial", "Stage1", "Stage2" };
	Begin("Editor");

	Checkbox("TextInputMode", &m_isTextInputMode);

	if (Combo("Stage", &m_Curr_Stage, szStages, IM_ARRAYSIZE(szStages)))
	{
		m_PropList.clear();
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
	static _float fTrigger_Size{};
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
		InputInt("Trigger_Number", &m_iTriggerNum);

		if (Button("Export_Monster"))
		{
			Export_Monster_Data();
		}

		m_eItemType = ItemType::Monster;
		EndTabItem();
	}
	if (BeginTabItem("NPC"))
	{
		ListBox("NPC", &iCurrListIndex, m_pItemList_NPC, IM_ARRAYSIZE(m_pItemList_NPC));

		m_eItemType = ItemType::NPC;
		EndTabItem();
	}
	if (BeginTabItem("Trigger"))
	{
		m_eItemType = ItemType::Trigger;

		ListBox("Triggers", &iCurrListIndex, m_TriggerItemList.data(), m_TriggerItemList.size());

		if (Button("Create Trigger"))
		{
			m_TriggerList.push_back(_float3(m_vPos.x, m_vPos.y, m_vPos.z));
			_char* pBuffer = new _char[MAX_PATH];
			snprintf(pBuffer, MAX_PATH, "X:%.1f, Y:%.1f, Z:%.1f", m_vPos.x, m_vPos.y, m_vPos.z);
			m_TriggerItemList.push_back(pBuffer);
			Create_TriggerDummy();
		}

		if (Button("Delete Trigger"))
		{
			{
				auto iter = m_TriggerList.begin();
				for (_int i = 0; i < iCurrListIndex; i++)
				{
					iter++;
				}
				m_TriggerList.erase(iter);
			}

			{
				Safe_Delete_Array(m_TriggerItemList[iCurrListIndex]);
				auto iter = m_TriggerItemList.begin();
				iter += iCurrListIndex;
				m_TriggerItemList.erase(iter);
			}
		}

		if (Button("Export_Trigger"))
		{
			Export_Trigger_Data();
		}

		EndTabItem();
	}
	if (BeginTabItem("Camera"))
	{
		static char buf[32] = "";
		ImGui::InputText("Scene_Name", buf, 32, ImGuiInputTextFlags_CharsNoBlank);

		ListBox("Scenes", &iCurrListIndex, m_SceneList.data(), m_SceneList.size());

		if (Button("Rec") or not m_isTextInputMode and m_pGameInstance->Key_Down(DIK_R))
		{
			if (not m_isRecording)
			{
				m_isRecording = true;
				m_fRecTimer = {};
				CUTSCENE* pScene = new CUTSCENE;
				m_Scenes.push_back(pScene);
			}
			else
			{
				m_isRecording = false;
				_char* pBuffer = new _char[32];
				strcpy_s(pBuffer, 32, buf);
				m_SceneList.push_back(pBuffer);
			}
		}SameLine();

		if (Button("Play"))
		{
			m_PlayScene = true;
			m_pCurrentScene = m_Scenes[iCurrListIndex];
		}

		if (Button("Export"))
		{
			filesystem::path strFilePath = "../../Client/Bin/Resources/CutScenes/" + string(m_SceneList[iCurrListIndex]) + ".hyuntrascene";

			ofstream OutFile(strFilePath.c_str(), ios::binary);

			if (OutFile.is_open())
			{
				size_t SceneSize = m_Scenes[iCurrListIndex]->size();
				OutFile.write(reinterpret_cast<_char*>(&SceneSize), sizeof size_t);

				for (auto& Pair : *m_Scenes[iCurrListIndex])
				{
					OutFile.write(reinterpret_cast<const _char*>(&Pair.first), sizeof _float4);
					OutFile.write(reinterpret_cast<const _char*>(&Pair.second), sizeof _float4);
				}
				OutFile.close();
			}
		}SameLine();

		if (Button("Import"))
		{
			filesystem::path strFilePath = "../../Client/Bin/Resources/CutScenes/" + string(buf) + ".hyuntrascene";

			ifstream ImportFile(strFilePath.c_str(), ios::binary);

			if (ImportFile.is_open())
			{
				size_t SceneSize{};
				ImportFile.read(reinterpret_cast<_char*>(&SceneSize), sizeof size_t);

				CUTSCENE* pScene = new CUTSCENE;
				m_Scenes.push_back(pScene);

				_float4 vPos{}, vLook{};

				for (size_t i = 0; i < SceneSize; i++)
				{
					ImportFile.read(reinterpret_cast<_char*>(&vPos), sizeof _float4);
					ImportFile.read(reinterpret_cast<_char*>(&vLook), sizeof _float4);
					m_Scenes.back()->push_back({ vPos, vLook });
				}
				_char* pBuffer = new _char[32];
				strcpy_s(pBuffer, 32, buf);
				m_SceneList.push_back(pBuffer);

				ImportFile.close();
			}
		}
		EndTabItem();
	}
	EndTabBar();

	if (m_isRecording and m_fRecTimer > 0.25f)
	{
		m_fRecTimer = {};
		m_Scenes.back()->push_back({ m_pGameInstance->Get_CameraPos(), m_pGameInstance->Get_CameraLook() });
	}
	m_fRecTimer += fTimeDelta;

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

	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL) and m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		m_ComputeSelection = true;
		m_fCamDist = -1.f;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = nullptr;
	}
	else
	{
		m_ComputeSelection = false;
	}

	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
	{
		m_ComputePickPos = true;
		m_fCamDist = -1.f;
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
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}
	if (m_pGameInstance->Key_Down(DIK_DOWN) || m_pGameInstance->Gamepad_Down(XINPUT_DOWN))
	{
		m_vPos.z -= fHorizental_Step;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT) || m_pGameInstance->Gamepad_Down(XINPUT_RIGHT))
	{
		m_vPos.x += fHorizental_Step;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}
	if (m_pGameInstance->Key_Down(DIK_LEFT) || m_pGameInstance->Gamepad_Down(XINPUT_LEFT))
	{
		m_vPos.x -= fHorizental_Step;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}
	if (m_pGameInstance->Key_Down(DIK_RSHIFT))
	{
		m_vPos.y += fVertical_Step;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}
	if (m_pGameInstance->Key_Down(DIK_RCONTROL))
	{
		m_vPos.y -= fVertical_Step;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}
	}

	SeparatorText("Look");

	Tip("Press 'V' to Set Look from Marker to Cursor. Normalizes Automatically");
	InputFloat3("Look", (_float*)&m_vLook, "%.2f"); SameLine();
	if (Button("Normalize"))
	{
		XMStoreFloat4(&m_vLook, XMVector4Normalize(XMLoadFloat4(&m_vLook)));
	}

	if (not m_isTextInputMode and m_pGameInstance->Key_Pressing(DIK_V))
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

	if (Button("Create") || not m_isTextInputMode and m_pGameInstance->Key_Down(DIK_C) || m_pGameInstance->Gamepad_Down(XINPUT_RB))
	{
		Create_Dummy(iCurrListIndex);
		m_pGameInstance->Gamepad_Vibration(32000, 16000);
	} SameLine();
	if (Button("Modify") || m_pGameInstance->Key_Down(DIK_M))
	{
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Modify(XMLoadFloat4(&m_vPos), XMLoadFloat4(&m_vLook));
		}

	} SameLine();
	if (Button("Delete") || not m_isTextInputMode and m_pGameInstance->Key_Down(DIK_F))
	{
		Delete_Dummy();
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

HRESULT CImguiMgr::Ready_Layers(vector<string>* PropList)
{
	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(-10.f, 15.f, -10.f, 1.f);
	CamDesc.vFocusPos = _float4(10.f, 0.f, 10.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 70000.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &m_vPos)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Sky"), TEXT("Prototype_GameObject_Sky"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Cursor"), TEXT("Prototype_GameObject_Cursor"), this)))
	{
		return E_FAIL;
	}

	m_iNumProps = static_cast<int>(PropList->size());
	m_pItemList_Props = new const _char * [m_iNumProps];

	for (size_t i = 0; i < m_iNumProps; i++)
	{
		m_pItemList_Props[i] = (*PropList)[i].c_str();
	}

	return S_OK;
}

void CImguiMgr::Create_Dummy(const _int& iListIndex)
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Select(false);
		m_pSelectedDummy = nullptr;
	}

	DummyInfo Info{};

	Info.ppDummy = &m_pSelectedDummy;
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
		Info.iTriggerNum = m_iTriggerNum;
		break;
	}
	Info.Prototype += strUnicode;
	Info.iStageIndex = m_Curr_Stage;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : Dummy");
	}

	switch (m_eItemType)
	{
	case MapEditor::ItemType::Props:
		m_PropList.push_back(m_pSelectedDummy);
		break;
	case MapEditor::ItemType::Monster:
		m_MonsterList.push_back(m_pSelectedDummy);
		break;
	}
}

void CImguiMgr::Create_TriggerDummy()
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Select(false);
		m_pSelectedDummy = nullptr;
	}

	DummyInfo Info{};

	Info.ppDummy = &m_pSelectedDummy;
	Info.vPos = m_vPos;
	Info.vLook = _float4(0.f, 0.f, 1.f, 0.f);
	Info.Prototype = L"Prototype_Model_Trigger";
	Info.eType = m_eItemType;
	Info.iStageIndex = m_Curr_Stage;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : Dummy");
	}
}

void CImguiMgr::Delete_Dummy()
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Kill();

		for (auto iter = m_PropList.begin(); iter != m_PropList.end();)
		{
			if (*iter == m_pSelectedDummy)
			{
				iter = m_PropList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}

HRESULT CImguiMgr::Load_Data()
{
	DummyInfo Info{};
	Info.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	Info.vLook = _float4(0.f, 0.f, 1.f, 0.f);
	Info.eType = ItemType::Map;
	Info.iStageIndex = m_Curr_Stage;
	_int i{};

	for (auto& strFileName : m_pMapModels[m_Curr_Stage])
	{
		Info.Prototype = L"Prototype_Model_";
		Info.Prototype += strFileName;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

	Info.eType = ItemType::COLMap;
	for (auto& strFileName : m_pMapCOLModels[m_Curr_Stage])
	{
		Info.Prototype = L"Prototype_Model_";
		Info.Prototype += strFileName;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

	{
		filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/" + std::to_wstring(m_Curr_Stage) + L".hyuntramap";

		ifstream InFile(strFilePath.c_str(), ios::binary);

		if (InFile.is_open())
		{
			m_PropList.clear();

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
				InFile.read(reinterpret_cast<_char*>(&Info.iStageIndex), sizeof _uint);
				InFile.read(reinterpret_cast<_char*>(&Info.vPos), sizeof _float4);
				InFile.read(reinterpret_cast<_char*>(&Info.vLook), sizeof _float4);
				Info.ppDummy = &m_pSelectedDummy;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
				{
					MSG_BOX("Failed to Add Layer : Dummy");
				}
				else
				{
					m_PropList.push_back(m_pSelectedDummy);
				}
			}
			InFile.close();
		}
	}

	{
		filesystem::path strFilePath = TEXT("../../Client/Bin/Resources/MapData/Trigger_") + std::to_wstring(m_Curr_Stage) + TEXT(".hyuntratrigger");

		ifstream File(strFilePath.c_str(), ios::binary);

		m_TriggerItemList.clear();
		m_TriggerList.clear();

		if (File.is_open())
		{

			size_t TriggerSize{};
			File.read(reinterpret_cast<_char*>(&TriggerSize), sizeof size_t);
			for (size_t i = 0; i < TriggerSize; i++)
			{
				DummyInfo Info{};

				File.read(reinterpret_cast<_char*>(&Info.vPos), sizeof _float3);
				Info.vPos.w = 1.f;

				m_TriggerList.push_back(_float3(Info.vPos.x, Info.vPos.y, Info.vPos.z));
				_char* pBuffer = new _char[MAX_PATH];
				snprintf(pBuffer, MAX_PATH, "X:%.1f, Y:%.1f, Z:%.1f", Info.vPos.x, Info.vPos.y, Info.vPos.z);
				m_TriggerItemList.push_back(pBuffer);

				Info.ppDummy = &m_pSelectedDummy;
				Info.vLook = _float4(0.f, 0.f, 1.f, 0.f);
				Info.Prototype = L"Prototype_Model_Trigger";
				Info.eType = ItemType::Trigger;
				Info.iStageIndex = m_Curr_Stage;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
				{
					MSG_BOX("Failed to Add Layer : Dummy");
				}
			}
		}
	}

	return S_OK;
}

HRESULT CImguiMgr::Export_Data()
{
	filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/" + std::to_wstring(m_Curr_Stage) + L".hyuntramap";

	ofstream OutFile(strFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		size_t DummySize = m_PropList.size();
		OutFile.write(reinterpret_cast<_char*>(&DummySize), sizeof size_t);

		for (auto& Data : m_PropList)
		{
			Data->WriteFile(OutFile);
		}
		OutFile.close();
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CImguiMgr::Export_Trigger_Data()
{
	filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/Trigger_" + std::to_wstring(m_Curr_Stage) + L".hyuntratrigger";

	ofstream OutFile(strFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		size_t TriggerSize = m_TriggerList.size();
		OutFile.write(reinterpret_cast<const _char*>(&TriggerSize), sizeof size_t);

		for (auto& vPos : m_TriggerList)
		{
			OutFile.write(reinterpret_cast<const _char*>(&vPos), sizeof _float3);
		}
		OutFile.close();
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CImguiMgr::Export_Monster_Data()
{
	filesystem::path strFilePath = L"../../Client/Bin/Resources/MapData/Monster_" + std::to_wstring(m_Curr_Stage) + L".hyuntramonster";

	ofstream OutFile(strFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		size_t DummySize = m_MonsterList.size();
		OutFile.write(reinterpret_cast<_char*>(&DummySize), sizeof size_t);

		for (auto& Data : m_MonsterList)
		{
			Data->WriteMonsterFile(OutFile);
		}
		OutFile.close();
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

void CImguiMgr::Free()
{
	for (auto pBuffer : m_TriggerItemList)
	{
		Safe_Delete_Array(pBuffer);
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();

}
