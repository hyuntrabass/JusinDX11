#pragma once
#include "Base.h"

BEGIN(Engine)

class CPhysX final : public CBase
{
private:
	CPhysX();
	virtual ~CPhysX() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);

public:
	void Init_Dynamic_PhysX(class CTransform* pTransform);
	void Init_Static_PhysX(class CTransform* pTransform);
	void Fetch_PhysX(class CTransform* pTransform);
	void Update_PhysX(class CTransform* pTransform);

private:
	PxFoundation* m_pFoundation{ nullptr };
	//PxPvd* m_pPvd{ nullptr };
	PxPhysics* m_pPhysics{ nullptr };

	PxScene* m_pScene{ nullptr };
	PxDefaultCpuDispatcher* m_pDispatcher{ nullptr };
	PxMaterial* m_pMaterial{ nullptr };

	map<class CTransform*, PxRigidDynamic*> m_DynamicActors{};
	map<class CTransform*, PxRigidStatic*> m_StaticActors{};

public:
	static CPhysX* Create();
	virtual void Free() override;
};

END