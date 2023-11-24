#include "Level_Stage2.h"

CLevel_Stage2::CLevel_Stage2(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_STAGE2);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	pPlayerTransform->Set_Position(_float3(-4.5f, 36.1f, -56.f));

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
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
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Forest/Mesh/";
	wstring strPrototypeTag = L"Prototype_Model_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strFileName = entry.path().stem().wstring();
			ObjectInfo Info{};
			Info.strPrototypeTag = strPrototypeTag + strFileName;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE2, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
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

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE2, L"Layer_Terrain_COL", L"Prototype_GameObject_MapCollider", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE2, L"Layer_Boss", L"Prototype_GameObject_Kurama")))
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
