#include "Level_Stage1.h"
#include "Level_Loading.h"

CLevel_Stage1::CLevel_Stage1(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(ToIndex(Level_ID::Stage1));

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
	}

	return S_OK;
}

void CLevel_Stage1::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Stage2))))
		{
			return;
		}
	}
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

			if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Stage1), L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}

		}
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
