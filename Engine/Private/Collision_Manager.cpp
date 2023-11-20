#include "Collision_Manager.h"
#include "GameObject.h"

void CCollision_Manager::Register_Actor(CTransform* pTransform, CGameObject* pObject, _bool isPlayer)
{
	if (isPlayer)
	{
		m_pPlayer = pObject;
		m_pPlayerActor = pTransform->Get_Controller()->getActor();
	}
	else
	{
		m_Monsters.emplace(pTransform->Get_Controller()->getActor(), pObject);
	}
}

HRESULT CCollision_Manager::Attach_Trigger(CTransform* pTransform, _float3 TriggerSize, _float44 OffsetMatrix, _int iDamage)
{
	// #TODO 치고 맞는 충돌
	
	//// 컨트롤러의 위치 정보를 얻습니다.
	//PxExtendedVec3 controllerPosition = controller->getFootPosition();

	//// 캐릭터가 바라보는 방향을 결정합니다.
	//// 이 예에서는 카메라의 방향을 사용하였습니다.
	//PxQuat characterRotation = camera->getRotation();

	//// 캐릭터가 바라보는 방향으로 일정 거리를 계산합니다.
	//PxVec3 offset = characterRotation.rotate(PxVec3(0, 0, -1)) * attackDistance;

	//// 충돌체의 위치를 설정합니다.
	//PxVec3 shapePosition = PxVec3(controllerPosition.x, controllerPosition.y, controllerPosition.z) + offset;

	//// 충돌체의 위치와 방향을 설정합니다.
	//PxTransform shapeTransform(shapePosition, characterRotation);

	//// 충돌체를 생성합니다.
	//PxShape* shape = actor->createShape(PxBoxGeometry(attackWidth, attackHeight, attackDepth), *material);
	//shape->setLocalPose(shapeTransform);

	return S_OK;
}

HRESULT CCollision_Manager::Detach_Trigger(CTransform* pTransform)
{
	auto iter = m_Triggers.find(pTransform);
	if (iter == m_Triggers.end())
	{
		return E_FAIL;;
	}

	pTransform->Get_Controller()->getActor()->detachShape(*iter->second);

	return S_OK;
}

void CCollision_Manager::Intersect(PxActor* pActor)
{
	if (pActor == m_pPlayerActor)
	{
		m_pPlayer->Set_Damage(10);
	}
	else
	{
		auto iter = m_Monsters.find(pActor);
		if (iter == m_Monsters.end())
		{
			MSG_BOX("말도 안돼");
		}

		iter->second->Set_Damage(10);
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	return new CCollision_Manager();
}

void CCollision_Manager::Free()
{

}
