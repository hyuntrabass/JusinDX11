#include "Collision_Manager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

void CCollision_Manager::RegisterTrigger(TriggerDesc Desc)
{
	m_Triggers.emplace(Desc.pActor, Desc);
}

void CCollision_Manager::EnterTrigger(PxTriggerPair* pPair)
{
	auto Trigger = m_Triggers.find(pPair->triggerActor);
	if (Trigger == m_Triggers.end())
	{
		return;
	}

	for (auto iter = Trigger->second.CollidedActors.begin(); iter != Trigger->second.CollidedActors.end(); ++iter)
	{
		if (*iter == pPair->otherActor)
		{
			return;
		}
	}

	Trigger->second.CollidedActors.push_back(pPair->otherActor);
}

void CCollision_Manager::LeaveTrigger(PxTriggerPair* pPair)
{
	auto Trigger = m_Triggers.find(pPair->triggerActor);
	if (Trigger == m_Triggers.end())
	{
		return;
	}

	for (auto iter = Trigger->second.CollidedActors.begin(); iter != Trigger->second.CollidedActors.end(); ++iter)
	{
		if (*iter == pPair->otherActor)
		{
			Trigger->second.CollidedActors.erase(iter);
			return;
		}
	}
}

_bool CCollision_Manager::IsCollided(CTransform* pTransform)
{
	PxActor* pActor{ pTransform->Get_Controller()->getActor() };

	for (auto& pair : m_Triggers)
	{
		for (auto& pOtherActor : pair.second.CollidedActors)
		{
			if (pActor == pOtherActor)
			{
				return true;
			}
		}
	}

	return false;
}

void CCollision_Manager::Free()
{

}
