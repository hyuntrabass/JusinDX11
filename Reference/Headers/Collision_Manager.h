#pragma once
#include "Base.h"

BEGIN(Engine)

enum class TriggerType
{
	Player,
	Monster,
};

struct TriggerDesc
{
	TriggerType eType{};

	PxActor* pActor{};
	_int iDamage{};

	list<PxActor*> CollidedActors{};
};

class ENGINE_DLL CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)
private:
	CCollision_Manager() = default;
	virtual ~CCollision_Manager() = default;

public:
	void RegisterTrigger(TriggerDesc Desc);
	void EnterTrigger(PxTriggerPair* pPair);
	void LeaveTrigger(PxTriggerPair* pPair);
	_bool IsCollided(class CTransform* pTransform);

private:
	map<PxActor*, TriggerDesc> m_Triggers{};
	//map<class CTransform*, PxShape*> m_Triggers{};
	//map<PxActor*, class CGameObject*> m_Monsters{};
	//PxActor* m_pPlayerActor{ nullptr };
	//class CGameObject* m_pPlayer{ nullptr };

public:
	virtual void Free() override;
};

END