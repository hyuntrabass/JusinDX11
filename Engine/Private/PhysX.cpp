#include "PhysX.h"
#include "Transform.h"

CPhysX::CPhysX()
{
}

HRESULT CPhysX::Init()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!m_pFoundation)
	{
		return E_FAIL;
	}

	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);
	if (!m_pPhysics)
	{
		return E_FAIL;
	}

	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(1);
	SceneDesc.cpuDispatcher = m_pDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pScene = m_pPhysics->createScene(SceneDesc);
	if (!m_pScene)
	{
		return E_FAIL;
	}

	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	//PxTransform pose = PxTransform(PxVec3(0.0f, -15.0f, 0.0f));
	//PxShape* shape = m_pPhysics->createShape(PxPlaneGeometry(), *m_pMaterial);
	//pGround->attachShape(*shape);
	//shape->release();
	PxRigidStatic* pGround = PxCreatePlane(*m_pPhysics, PxPlane(0.f, 1.f, 0.f, 0.f), *m_pMaterial);
	m_pScene->addActor(*pGround);

	return S_OK;
}

void CPhysX::Tick(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);
}

void CPhysX::Init_Dynamic_PhysX(CTransform* pTransform)
{
	_float44 World = pTransform->Get_World_float4x4();
	PxMat44 WorldMat = PxMat44(reinterpret_cast<_float*>(&World));
	PxTransform Pose = PxTransform(WorldMat);

	PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *m_pMaterial);
	PxRigidDynamic* pActor = m_pPhysics->createRigidDynamic(Pose);
	pActor->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*pActor, 10.f);
	m_pScene->addActor(*pActor);

	m_DynamicActors.emplace(pTransform, pActor);

	pShape->release();
}

void CPhysX::Init_Static_PhysX(CTransform* pTransform)
{
}

void CPhysX::Fetch_PhysX(CTransform* pTransform)
{
	auto iter = m_DynamicActors.find(pTransform);
	PxTransform Pose = iter->second->getGlobalPose();

	pTransform->Set_Rotation(XMVectorSet(Pose.q.x, Pose.q.y, Pose.q.z, Pose.q.w));
	pTransform->Set_State(State::Pos, XMVectorSet(Pose.p.x, Pose.p.y, Pose.p.z, 1.f));
}

void CPhysX::Update_PhysX(CTransform* pTransform)
{
	auto iter = m_DynamicActors.find(pTransform);
	_float44 World = pTransform->Get_World_float4x4();
	PxMat44 WorldMat = PxMat44(reinterpret_cast<_float*>(&World));
	PxTransform Pose = PxTransform(WorldMat);

	iter->second->setGlobalPose(Pose);
}

CPhysX* CPhysX::Create()
{
	CPhysX* pInstance = new CPhysX();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CPhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysX::Free()
{
	m_pMaterial->release();
	m_pDispatcher->release();
	m_pScene->release();
	m_pPhysics->release();
	m_pFoundation->release();
}
