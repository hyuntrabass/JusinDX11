#include "Collision_Manager.h"
#include "GameObject.h"
#include "Collider.h"

HRESULT CCollision_Manager::Register_CollisionObject(CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer)
{
	if (not pObject or not pHitCollider)
	{
		return E_FAIL;
	}

	if (IsPlayer)
	{
		if (m_pPlayer)
		{
			MSG_BOX("Player Already Exist! : Collision_Manager");
			return E_FAIL;
		}
		m_pPlayer = pObject;
		m_pPlayerHitCollider = pHitCollider;

		Safe_AddRef(m_pPlayer);
		Safe_AddRef(m_pPlayerHitCollider);
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			MSG_BOX("Monster Already Exist! : Collision_Manager");
			return E_FAIL;
		}

		m_Monsters.emplace(pObject, pHitCollider);
		Safe_AddRef(pObject);
		Safe_AddRef(pHitCollider);
	}

	return S_OK;
}

void CCollision_Manager::Delete_CollisionObject(CGameObject* pObject, _bool IsPlayer)
{
	if (IsPlayer)
	{
		Safe_Release(m_pPlayer);
		Safe_Release(m_pPlayerHitCollider);

		m_pPlayer = nullptr;
		m_pPlayerHitCollider = nullptr;
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			Safe_Release(const_cast<CGameObject*>(iter->first));
			Safe_Release(iter->second);
			m_Monsters.erase(iter);
		}
	}
}

void CCollision_Manager::Attack_Monster(CCollider* pCollider, _uint iDamage)
{
	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(pCollider))
		{
			Monster.first->Set_Damage(iDamage);
		}
	}
}

void CCollision_Manager::Attack_Player(CCollider* pCollider, _uint iDamage)
{
	if (pCollider->Intersect(m_pPlayerHitCollider))
	{
		m_pPlayer->Set_Damage(iDamage);
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	return pInstance;
}

void CCollision_Manager::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerHitCollider);

	for (auto& Monster : m_Monsters)
	{
		Safe_Release(Monster.second);
		Safe_Release(const_cast<CGameObject*>(Monster.first));
	}
	m_Monsters.clear();
}
