#include "Level_Tutorial.h"
#include "Level_Loading.h"

CLevel_Tutorial::CLevel_Tutorial(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_TUTORIAL);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	pPlayerTransform->Set_Position(_float3(40.f, 32.f, 126.f));

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

	return S_OK;
}

void CLevel_Tutorial::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR) || m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
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

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TUTORIAL, L"Layer_Terrain", L"Prototype_GameObject_Terrain", &Info)))
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

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TUTORIAL, L"Layer_Terrain_COL", L"Prototype_GameObject_MapCollider", &Info)))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Sky", L"Prototype_GameObject_Sky")))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_TUTORIAL, LightDesc);
}

HRESULT CLevel_Tutorial::Ready_UIs()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI_HpBar"), TEXT("Prototype_GameObject_UI_HpBar"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI_HpBar_Base"), TEXT("Prototype_GameObject_UI_HpBar_Base"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI_HpBar_Base"), TEXT("Prototype_GameObject_UI_SlotBase_Skill"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI_HpBar_Base"), TEXT("Prototype_GameObject_UI_HpBar_SlotBase_Tool"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_People()
{
	for (size_t i = 0; i < 10; i++)
	{
		_float4 vPos{};
		vPos.x = -11 + (2.f * (rand() % 7));
		vPos.y = 50.f;
		vPos.z = 85.f + (2.f * (rand() % 10));
		vPos.w = 1.f;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TUTORIAL, L"Layer_Sandman", L"Prototype_GameObject_Sandman", &vPos)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TUTORIAL, L"Layer_Kakashi", L"Prototype_GameObject_Kakashi")))
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
