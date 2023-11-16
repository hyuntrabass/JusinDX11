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
	//// ��Ʈ�ѷ��� ��ġ ������ ����ϴ�.
	//PxExtendedVec3 controllerPosition = controller->getFootPosition();

	//// ĳ���Ͱ� �ٶ󺸴� ������ �����մϴ�.
	//// �� �������� ī�޶��� ������ ����Ͽ����ϴ�.
	//PxQuat characterRotation = camera->getRotation();

	//// ĳ���Ͱ� �ٶ󺸴� �������� ���� �Ÿ��� ����մϴ�.
	//PxVec3 offset = characterRotation.rotate(PxVec3(0, 0, -1)) * attackDistance;

	//// �浹ü�� ��ġ�� �����մϴ�.
	//PxVec3 shapePosition = PxVec3(controllerPosition.x, controllerPosition.y, controllerPosition.z) + offset;

	//// �浹ü�� ��ġ�� ������ �����մϴ�.
	//PxTransform shapeTransform(shapePosition, characterRotation);

	//// �浹ü�� �����մϴ�.
	//PxShape* shape = actor->createShape(PxBoxGeometry(attackWidth, attackHeight, attackDepth), *material);
	//shape->setLocalPose(shapeTransform);


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
			MSG_BOX("���� �ȵ�");
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
