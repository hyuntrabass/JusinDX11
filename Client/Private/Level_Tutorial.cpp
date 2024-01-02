#include "Level_Tutorial.h"
#include "Level_Loading.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"

CLevel_Tutorial::CLevel_Tutorial(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_FOREST);
	m_pGameInstance->Set_FogNF(_float2(30.f, 300.f));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	CTransform* pMainCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));

	pPlayerTransform->Set_Position(_float3(40.f, 32.f, 126.f));
	pPlayerTransform->LookAt_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
	pMainCamTransform->LookAt_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f));

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
	}

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("Failed to Ready Lights");
	}

	if (FAILED(Ready_UIs()))
	{
		MSG_BOX("Failed to Ready UIs");
	}

	if (FAILED(Ready_People()))
	{
		MSG_BOX("Failed to Ready People");
	}

	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("Forest_Ambience"), 0.5f);

	m_pGameInstance->Set_HellHeight(20.f);

	return S_OK;
}

void CLevel_Tutorial::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR) || m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE))))
		{
			return;
		}
		return;
	}

	//m_pGameInstance->PhysXTick(fTimeDelta);
	CTrigger_Manager::Get_Instance()->Tick(fTimeDelta);
}

HRESULT CLevel_Tutorial::Render()
{
	//if (FAILED(m_pGameInstance->Render_PhysX()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Map()
{
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Forest/Mesh/";
	wstring strPrototypeTag = L"Prototype_Model_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_FOREST, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Forest/COL_Mesh/";
	strPrototypeTag = L"Prototype_Model_COL_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_FOREST, L"Layer_Terrain_COL", L"Prototype_GameObject_MapCollider", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/0.hyuntramap");

	ifstream InFile(strFilePath.c_str(), ios::binary);

	if (InFile.is_open())
	{
		size_t PropSize{};
		ItemType Type{};
		_uint iTrash{};
		InFile.read(reinterpret_cast<_char*>(&PropSize), sizeof size_t);

		for (size_t i = 0; i < PropSize; i++)
		{
			ObjectInfo Info{};
			size_t NameSize{};

			InFile.read(reinterpret_cast<_char*>(&NameSize), sizeof size_t);
			wchar_t* pBuffer = new wchar_t[NameSize / sizeof(wchar_t)];
			InFile.read(reinterpret_cast<_char*>(pBuffer), NameSize);
			Info.strPrototypeTag = pBuffer;
			Safe_Delete_Array(pBuffer);
			InFile.read(reinterpret_cast<_char*>(&Type), sizeof ItemType);
			InFile.read(reinterpret_cast<_char*>(&iTrash), sizeof _uint);
			InFile.read(reinterpret_cast<_char*>(&Info.vPos), sizeof _float4);
			InFile.read(reinterpret_cast<_char*>(&Info.vLook), sizeof _float4);

			if (Type != ItemType::Props)
			{
				continue;
			}

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_FOREST, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}
		}
		InFile.close();
	}
	else
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Sky", L"Prototype_GameObject_Sky")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_FOREST, L"Layer_Hell", L"Prototype_GameObject_Hell")))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.2f, 0.f);
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_FOREST, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_UIs()
{
	if (FAILED(CUI_Manager::Get_Instance()->Ready_UI_Tuto()))
	{
		MSG_BOX("Failed to Ready UI_Tuto");
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_People()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_FOREST, L"Layer_Kakashi", L"Prototype_GameObject_Kakashi")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Tutorial* CLevel_Tutorial::Create(_dev pDevice, _context pContext)
{
	CLevel_Tutorial* pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Tutorial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();
}
