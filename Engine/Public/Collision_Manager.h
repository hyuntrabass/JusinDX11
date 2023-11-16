#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager() = default;
	virtual ~CCollision_Manager() = default;

public:
	void Register_Actor(class CTransform* pTransform, class CGameObject* pObject, _bool isPlayer = false);
	HRESULT Attach_Trigger(class CTransform* pTransform, _float3 TriggerSize, _float44 OffsetMatrix, _int iDamage);
	HRESULT Detach_Trigger(class CTransform* pTransform);
	void Intersect(PxActor* pActor);

private:
	map<class CTransform*, PxShape*> m_Triggers{};
	map<PxActor*, class CGameObject*> m_Monsters{};
	PxActor* m_pPlayerActor{ nullptr };
	class CGameObject* m_pPlayer{ nullptr };

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END