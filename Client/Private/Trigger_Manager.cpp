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

void CTrigger_Manager::Register_PlayerCollider(CCollider* pCollider)
{
	m_pPlayerCollider = pCollider;
	Safe_AddRef(m_pPlayerCollider);
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
	switch (m_pGameInstance->Get_CurrentLevelIndex())
	{
	case LEVEL_TUTORIAL:
		Trigger_Tutorial(fTimeDelta);
		break;
	case LEVEL_STAGE1:
		Trigger_Village(fTimeDelta);
		break;
	case LEVEL_BOSSSTAGE:
		Trigger_Cloud(fTimeDelta);
		break;
	}
}

#ifdef _DEBUG
HRESULT CTrigger_Manager::Render()
{
	for (auto& Trigger : m_Triggers[m_pGameInstance->Get_CurrentLevelIndex()])
	{
		if (FAILED(Trigger->Render()))
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
	Desc.vCenter = _float3(40.f, 32.f, 126.f);
	Desc.fRadius = 1.f;
	if (pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), &Desc)))
	{
		m_Triggers[LEVEL_TUTORIAL].push_back(pCollider);
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

void CTrigger_Manager::Trigger_Tutorial(_float fTimeDelta)
{
	if (m_Triggers[LEVEL_TUTORIAL][0]->Intersect(m_pPlayerCollider))
	{

	}
}

void CTrigger_Manager::Trigger_Village(_float fTimeDelta)
{
	// 1 전투 구역
	//  CamPos X : -112.7049
	//	CamPos Y : 23.1608
	//	CamPos Z : 82.16119683

	//	CamLook X : 0.679019947
	//	CamLook Y : 0.1067018628
	//	CamLook Z : -0.726328318

	//	---
	//	CamPos X : -124.91263
	//	CamPos Y : 9.0392337
	//	CamPos Z : 65.47890333
	//	CamPos X : -124.65463
	//	CamPos Y : 8.8516537
	//	CamPos Z : 59.96750333
	//	CamPos X : -124.54663
	//	CamPos Y : 8.8516537
	//	CamPos Z : 55.38440333
	//	-- -
	//	CamPos X : -88.736763
	//	CamPos Y : 9.3991437
	//	CamPos Z : 90.56580333
	//	CamPos X : -98.494963
	//	CamPos Y : 8.6679337
	//	CamPos Z : 82.06280333
	//	-- -
	//	CamPos X : -60.451463
	//	CamPos Y : 10.231437
	//	CamPos Z : 61.81960333
	//	CamPos X : -68.154363
	//	CamPos Y : 8.3743337
	//	CamPos Z : 66.85740333
	//	-- -
	//	CamPos X : -81.413663
	//	CamPos Y : 7.9853837
	//	CamPos Z : 22.07440333
	//	CamPos X : -75.283863
	//	CamPos Y : 8.1619737
	//	CamPos Z : 32.79530333
	//	-- -
	// 
	// 2 전투 구역
	//	-- -
	//	CamPos X : 11.8373673
	//	CamPos Y : 18.824737
	//	CamPos Z : 75.72790333
	//	CamPos X : 0.28881773
	//	CamPos Y : 17.978537
	//	CamPos Z : 68.72090333
	//	-- -
	//	CamPos X : -4.40751281
	//	CamPos Y : 25.516637
	//	CamPos Z : -24.4558853
	//	CamPos X : 6.829993681
	//	CamPos Y : 19.153737
	//	CamPos Z : -5.26891853
	//	-- -
	// 
	// 3 전투 구역
	//	-- -
	//	CamPos X : 130.4962781
	//	CamPos Y : 18.231737
	//	CamPos Z : 104.4956163
	//	CamPos X : 106.3982781
	//	CamPos Y : 18.304237
	//	CamPos Z : 101.8246163
	//	-- -
	// 
	// 보스 전투 구역
	//	-- -
	//	CamPos X : 62.39672781
	//	CamPos Y : 38.428137
	//	CamPos Z : -81.4633596
	//	CamPos X : 89.27652781
	//	CamPos Y : 38.081137
	//	CamPos Z : -81.1606596
	//	-- -
	//	CamPos X : 105.9792781
	//	CamPos Y : 38.141337
	//	CamPos Z : -65.0155596
	//	CamPos X : 105.6812781
	//	CamPos Y : 37.775737
	//	CamPos Z : -40.3979596


	if (true)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STAGE1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Sandman"))))
		{
			MSG_BOX("Failed to Add Layer : Sandman");
		}
	}
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
			Safe_Release(pTrigger);
		}
		m_Triggers[i].clear();
	}
}
