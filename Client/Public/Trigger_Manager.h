#pragma once
#include "Client_Define.h"

BEGIN(Engine)
class CGameInstance;
class CCollider;
END

BEGIN(Client)

class CTrigger_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrigger_Manager)
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	const _bool& Hasto_PlayScene() const;
	void End_Scene();
	CUTSCENE* Get_CurrentScene();

	void Register_PlayerCollider(CCollider* pCollider);
	void Tick(_float fTimeDelta);
#ifdef _DEBUG
	HRESULT Render();
#endif // _DEBUG

private:
	CGameInstance* m_pGameInstance{ nullptr };

	vector<pair<CCollider*, _bool>> m_Triggers[LEVEL_END]{};
	CCollider* m_pPlayerCollider{ nullptr };

	vector<vector<ObjectInfo>> m_MonsterTriggers[LEVEL_END]{};

	map<const wstring, CUTSCENE> m_CutScenes{};
	CUTSCENE* m_pCurrentCutScene{ nullptr };
	_bool m_PlayScene{};

private:
	HRESULT Add_Triggers();

	void Trigger_Tutorial(_float fTimeDelta);
	void Trigger_Village(_float fTimeDelta);
	void Trigger_Cloud(_float fTimeDelta);

public:
	virtual void Free() override;
};

END