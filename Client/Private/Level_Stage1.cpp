#include "Level_Stage1.h"
#include "Level_Loading.h"
#include "Trigger_Manager.h"

CLevel_Stage1::CLevel_Stage1(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_STAGE1);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	pPlayerTransform->Set_Position(_float3(-112.7f, 24.f, 82.1f));
	pPlayerTransform->LookAt_Dir(XMVectorSet(0.679f, 0.f, -0.7263f, 0.f));
	//  CamPos X : -112.7049
//	CamPos Y : 23.1608
//	CamPos Z : 82.16119683

//	CamLook X : 0.679019947
//	CamLook Y : 0.1067018628
//	CamLook Z : -0.726328318

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Lights");
	}

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
	}

	return S_OK;
}

void CLevel_Stage1::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR) || m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSSSTAGE))))
		{
			return;
		}
	}

	CTrigger_Manager::Get_Instance()->Tick(fTimeDelta);
}

HRESULT CLevel_Stage1::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Map()
{
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Village/Mesh/";
	wstring strPrototypeTag = L"Prototype_Model_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE1, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE1, L"Layer_Boss", L"Prototype_GameObject_Pain")))
	{
		return E_FAIL;
	}

	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Village/COL_Mesh/";
	strPrototypeTag = L"Prototype_Model_COL_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE1, L"Layer_Terrain_COL", L"Prototype_GameObject_MapCollider", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/1.hyuntramap");

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

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE1, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
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
	
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.5f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STAGE1, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(_dev pDevice, _context pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Stage1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();
}
