#include "Level_BossStage.h"

CLevel_Stage2::CLevel_Stage2(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_BOSSSTAGE);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	pPlayerTransform->Set_Position(_float3(-45.3f, 72.6f, 106.3f));

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
	}

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("Failed to Ready Lights");
	}

	return S_OK;
}

void CLevel_Stage2::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_Stage2::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Map()
{
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Cloud/Mesh/";
	wstring strPrototypeTag = L"Prototype_Model_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_BOSSSTAGE, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}

		}
	}
	
	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Cloud/COL_Mesh/";
	strPrototypeTag = L"Prototype_Model_COL_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_BOSSSTAGE, L"Layer_Terrain_COL", L"Prototype_GameObject_MapCollider", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_BOSSSTAGE, L"Layer_Boss", L"Prototype_GameObject_Kurama")))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_BOSSSTAGE, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Stage2* CLevel_Stage2::Create(_dev pDevice, _context pContext)
{
	CLevel_Stage2* pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Stage2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();
}
