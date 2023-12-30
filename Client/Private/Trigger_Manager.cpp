#include "Trigger_Manager.h"
#include "GameInstance.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);

	if (FAILED(Add_Triggers()))
	{
		MSG_BOX("Failed to Add Triggers!");
	}
}

const _bool& CTrigger_Manager::Hasto_PlayScene() const
{
	return m_PlayScene;
}

void CTrigger_Manager::End_Scene()
{
	m_PlayScene = false;
	m_pCurrentCutScene = nullptr;
}

CUTSCENE* CTrigger_Manager::Get_CurrentScene()
{
	return m_pCurrentCutScene;
}

void CTrigger_Manager::Register_PlayerCollider(CCollider* pCollider)
{
	m_pPlayerCollider = pCollider;
	Safe_AddRef(m_pPlayerCollider);
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
	switch (m_pGameInstance->Get_CurrentLevelIndex())
	{
	case LEVEL_FOREST:
		Trigger_Tutorial(fTimeDelta);
		break;
	case LEVEL_VILLAGE:
		Trigger_Village(fTimeDelta);
		break;
	case LEVEL_CLOUD:
		Trigger_Cloud(fTimeDelta);
		break;
	}
}

#ifdef _DEBUG
HRESULT CTrigger_Manager::Render()
{
	for (auto& Trigger : m_Triggers[m_pGameInstance->Get_CurrentLevelIndex()])
	{
		if (FAILED(Trigger.first->Render()))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
#endif // _DEBUG


HRESULT CTrigger_Manager::Add_Triggers()
{
	CCollider* pCollider{ nullptr };
	Collider_Desc Desc{};
	Desc.eType = ColliderType::Sphere;

	// Tutorial
	{
		filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/Trigger_0.hyuntratrigger");

		ifstream File(strFilePath.c_str(), ios::binary);

		if (File.is_open())
		{
			size_t TriggerSize{};
			File.read(reinterpret_cast<_char*>(&TriggerSize), sizeof size_t);
			for (size_t i = 0; i < TriggerSize; i++)
			{
				File.read(reinterpret_cast<_char*>(&Desc.vCenter), sizeof _float3);

				Desc.fRadius = 5.f;
				pCollider = nullptr;
				if (pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), &Desc)))
				{
					m_Triggers[LEVEL_FOREST].push_back({ pCollider, false });
				}
				else
				{
					return E_FAIL;
				}

			}
		}
		else
		{
			return E_FAIL;
		}
	}

	// Village
	{
		filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/Trigger_1.hyuntratrigger");

		ifstream File(strFilePath.c_str(), ios::binary);

		if (File.is_open())
		{
			size_t TriggerSize{};
			File.read(reinterpret_cast<_char*>(&TriggerSize), sizeof size_t);
			for (size_t i = 0; i < TriggerSize; i++)
			{
				File.read(reinterpret_cast<_char*>(&Desc.vCenter), sizeof _float3);

				Desc.fRadius = 5.f;
				pCollider = nullptr;
				if (pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), &Desc)))
				{
					m_Triggers[LEVEL_VILLAGE].push_back({ pCollider, false });
					m_MonsterTriggers[LEVEL_VILLAGE].push_back(vector<ObjectInfo>());
				}
				else
				{
					return E_FAIL;
				}

			}
		}
		else
		{
			return E_FAIL;
		}
	}
	{
		filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/Monster_1.hyuntramonster");

		ifstream File(strFilePath.c_str(), ios::binary);

		if (File.is_open())
		{
			size_t NumMonter{};
			File.read(reinterpret_cast<_char*>(&NumMonter), sizeof size_t);
			for (size_t i = 0; i < NumMonter; i++)
			{
				ObjectInfo Info{};
				size_t NameSize{};
				_uint iTriggerNum{};

				File.read(reinterpret_cast<_char*>(&NameSize), sizeof size_t);
				wchar_t* pBuffer = new wchar_t[NameSize / sizeof(wchar_t)];
				File.read(reinterpret_cast<_char*>(pBuffer), NameSize);
				Info.strPrototypeTag = pBuffer;
				Safe_Delete_Array(pBuffer);

				File.read(reinterpret_cast<_char*>(&iTriggerNum), sizeof _uint);
				File.read(reinterpret_cast<_char*>(&Info.vPos), sizeof _float4);
				Info.vPos.y += 1.f;
				File.read(reinterpret_cast<_char*>(&Info.vLook), sizeof _float4);

				m_MonsterTriggers[LEVEL_VILLAGE][iTriggerNum].push_back(Info);
			}
		}
		else
		{
			return E_FAIL;
		}
	}

	// Cloud
	{
		filesystem::path strFilePath = TEXT("../Bin/Resources/MapData/Trigger_2.hyuntratrigger");

		ifstream File(strFilePath.c_str(), ios::binary);

		if (File.is_open())
		{
			size_t TriggerSize{};
			File.read(reinterpret_cast<_char*>(&TriggerSize), sizeof size_t);
			for (size_t i = 0; i < TriggerSize; i++)
			{
				File.read(reinterpret_cast<_char*>(&Desc.vCenter), sizeof _float3);

				Desc.fRadius = 5.f;
				pCollider = nullptr;
				if (pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), &Desc)))
				{
					m_Triggers[LEVEL_CLOUD].push_back({ pCollider, false });
				}
				else
				{
					return E_FAIL;
				}

			}
		}
		else
		{
			return E_FAIL;
		}
	}

	// CutScenes
	{
		filesystem::path strInputFilePath = "../Bin/Resources/Cutscenes/";
		for (const auto& entry : filesystem::recursive_directory_iterator(strInputFilePath))
		{
			if (entry.is_regular_file())
			{
				wstring strTag = L"Cutscene_";
				strTag += entry.path().stem().wstring();
				filesystem::path FullFilePath = strInputFilePath / entry.path().filename();

				CUTSCENE Scene{};
				ifstream File(FullFilePath, ios::binary);

				if (File.is_open())
				{
					size_t SceneSize{};

					File.read(reinterpret_cast<_char*>(&SceneSize), sizeof size_t);

					Scene.reserve(SceneSize);

					for (size_t i = 0; i < SceneSize; i++)
					{
						_float4 vPos{}, vLook{};
						File.read(reinterpret_cast<_char*>(&vPos), sizeof _float4);
						File.read(reinterpret_cast<_char*>(&vLook), sizeof _float4);

						Scene.push_back({ vPos, vLook });
					}

					m_CutScenes.emplace(strTag, Scene);
				}
			}
		}
	}

	return S_OK;
}

void CTrigger_Manager::Trigger_Tutorial(_float fTimeDelta)
{
	if (not m_Triggers[LEVEL_FOREST][0].second and m_Triggers[LEVEL_FOREST][0].first->Intersect(m_pPlayerCollider))
	{
		m_PlayScene = true;
		m_pCurrentCutScene = &m_CutScenes.find(TEXT("Cutscene_tuto1"))->second;
		m_Triggers[LEVEL_FOREST][0].second = true;
	}
}

void CTrigger_Manager::Trigger_Village(_float fTimeDelta)
{
	_float4 vOriginPos{};

	for (size_t i = 0; i < m_Triggers[LEVEL_VILLAGE].size(); i++)
	{
		auto& Trigger = m_Triggers[LEVEL_VILLAGE][i];

		if (not Trigger.second and Trigger.first->Intersect(m_pPlayerCollider))
		{
			Trigger.second = true;
		}

		if (Trigger.second and not m_MonsterTriggers[LEVEL_VILLAGE][i].empty())
		{
			auto& ObjInfo = m_MonsterTriggers[LEVEL_VILLAGE][i].back();
			
			if (ObjInfo.strPrototypeTag == TEXT("Prototype_Model_Sandman"))
			{
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Sandman"), &ObjInfo)))
				{
					MSG_BOX("Failed to Add Layer : Sandman");
				}
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_SandNinja"), &ObjInfo)))
				{
					MSG_BOX("Failed to Add Layer : SandNinja");
				}
			}
			
			m_MonsterTriggers[LEVEL_VILLAGE][i].pop_back();
		}
	}

	//if (not m_Triggers[LEVEL_VILLAGE][0].second and m_Triggers[LEVEL_VILLAGE][0].first->Intersect(m_pPlayerCollider))
	//{
	//	vOriginPos = { -124.9f, 9.f, 65.4789f, 1.f };
	//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Sandman"), &vOriginPos)))
	//	{
	//		MSG_BOX("Failed to Add Layer : Sandman");
	//	}

	//	vOriginPos = { -124.9f, 9.f, 60.4789f, 1.f };
	//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Sandman"), &vOriginPos)))
	//	{
	//		MSG_BOX("Failed to Add Layer : Sandman");
	//	}

	//	vOriginPos = { -124.9f, 9.f, 55.4789f, 1.f };
	//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Sandman"), &vOriginPos)))
	//	{
	//		MSG_BOX("Failed to Add Layer : Sandman");
	//	}

	//	m_Triggers[LEVEL_VILLAGE][0].second = true;
	//}
}

void CTrigger_Manager::Trigger_Cloud(_float fTimeDelta)
{
}

void CTrigger_Manager::Free()
{
	Safe_Release(m_pPlayerCollider);
	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < LEVEL_END; i++)
	{
		for (auto& pTrigger : m_Triggers[i])
		{
			Safe_Release(pTrigger.first);
		}
		m_Triggers[i].clear();
	}
}
