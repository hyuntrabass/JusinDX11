#include "Level_Tutorial.h"
#include "Level_Loading.h"

CLevel_Tutorial::CLevel_Tutorial(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(ToIndex(Level_ID::Tutorial));

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

void CLevel_Tutorial::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Stage1))))
		{
			return;
		}
	}
}

HRESULT CLevel_Tutorial::Render()
{
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

			if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Tutorial), L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Static), L"Layer_Sky", L"Prototype_GameObject_Sky")))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	return m_pGameInstance->Add_Light(ToIndex(Level_ID::Tutorial), LightDesc);
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
