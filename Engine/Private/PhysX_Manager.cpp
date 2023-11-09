#include "PhysX_Manager.h"
#include "Transform.h"
#include "VIBuffer_PhysX.h"
#include "Shader.h"
#include "GameInstance.h"

CPhysX_Manager::CPhysX_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysX_Manager::Init(CGameInstance* pGameInstance)
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
	PxTolerancesScale Scale{};

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, Scale, true);
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

	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.f);

	//PxTransform pose = PxTransform(PxVec3(0.0f, -15.0f, 0.0f));
	//PxShape* shape = m_pPhysics->createShape(PxPlaneGeometry(), *m_pMaterial);
	//pGround->attachShape(*shape);
	//shape->release();
	PxRigidStatic* pGround = PxCreatePlane(*m_pPhysics, PxPlane(0.f, 1.f, 0.f, 0.f), *m_pMaterial);
	m_pScene->addActor(*pGround);

#ifdef _DEBUG
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0);

	m_pDebugShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxColor.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements);
	if (!m_pDebugShader)
	{
		return E_FAIL;
	}
#endif // _DEBUG

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);
}

#ifdef _DEBUG
HRESULT CPhysX_Manager::Render()
{
	//if (!m_pVIBufferCom)
	//{
	//	return S_OK;
	//}
	//else
	//{
	//	const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//	m_pVIBufferCom->Update_Buffer(rb);
	//}

	//if (FAILED(m_pDebugShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pDebugShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pDebugShader->Begin(0)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pVIBufferCom->Render()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}
#endif // _DEBUG

void CPhysX_Manager::Init_PhysX_Character(CTransform* pTransform, CollisionGroup eGroup)
{
	_float3 vPos{};
	XMStoreFloat3(&vPos, pTransform->Get_State(State::Pos));
	PxExtendedVec3 Position{ static_cast<_double>(vPos.x), static_cast<_double>(vPos.y), static_cast<_double>(vPos.z) };

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.35f; // 위아래 반구의 반지름
	ControllerDesc.position = Position; // 초기 위치
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이
	ControllerDesc.reportCallback = nullptr;
	ControllerDesc.material = m_pMaterial;
	ControllerDesc.density = 700.f;

	PxController* pController = m_pControllerManager->createController(ControllerDesc);

	CollisionMask Mask{};
	switch (eGroup)
	{
	case Engine::COLGROUP_PLAYER:
		Mask = MASK_PLAYER;
		break;
	case Engine::COLGROUP_MONSTER:
		Mask = MASK_MONSTER;
		break;
	case Engine::COLGROUP_TERRAIN:
		Mask = MASK_TERRAIN;
		break;
	}

	pTransform->Set_Controller(pController);
	//m_Characters.emplace(pTransform, pController);
}

void CPhysX_Manager::Init_PhysX_MoveableObject(CTransform* pTransform)
{
	_float44 World = pTransform->Get_World_float4x4();
	PxMat44 WorldMat = PxMat44(reinterpret_cast<_float*>(&World));
	PxTransform Pose = PxTransform(WorldMat);

	PxShape* pShape = m_pPhysics->createShape(PxCapsuleGeometry(0.3f, 1.8f), *m_pMaterial);
	PxRigidDynamic* pActor = m_pPhysics->createRigidDynamic(Pose);
	pActor->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*pActor, 10.f);
	//pActor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
	m_pScene->addActor(*pActor);

	m_DynamicActors.emplace(pTransform, pActor);

#ifdef _DEBUG
	//if (m_pVIBufferCom)
	//{
	//	const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//	m_pVIBufferCom->Update_Buffer(rb);
	//}
	//else
	//{
	//	const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//	int a = rb.getNbLines();
	//	m_pVIBufferCom = CVIBuffer_PhysX::Create(m_pDevice, m_pContext, rb);
	//}
#endif // _DEBUG

	pShape->release();
}

void CPhysX_Manager::Apply_PhysX(CTransform* pTransform)
{
	auto iter = m_DynamicActors.find(pTransform);
	PxTransform Pose = iter->second->getGlobalPose();

	pTransform->Set_Rotation(XMVectorSet(Pose.q.x, Pose.q.y, Pose.q.z, Pose.q.w));
	pTransform->Set_State(State::Pos, XMVectorSet(Pose.p.x, Pose.p.y, Pose.p.z, 1.f));
}

void CPhysX_Manager::Update_PhysX(CTransform* pTransform)
{
	auto iter = m_DynamicActors.find(pTransform);
	_float44 World = pTransform->Get_World_float4x4();
	PxMat44 WorldMat = PxMat44(reinterpret_cast<_float*>(&World));
	PxTransform Pose = PxTransform(WorldMat);

	iter->second->setGlobalPose(Pose);
}

PxRigidStatic* CPhysX_Manager::Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices)
{
	PxTolerancesScale Scale{};

	PxCookingParams CookingParams(Scale);
	CookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	CookingParams.meshWeldTolerance = 0.001f;
	CookingParams.midphaseDesc = PxMeshMidPhase::eBVH34;

	PxTriangleMeshDesc MeshDesc{};
	MeshDesc.points.count = iNumVertices;
	MeshDesc.points.stride = sizeof(_float3);
	MeshDesc.points.data = pVertices;

	MeshDesc.triangles.count = iNumIndices / 3;
	MeshDesc.triangles.stride = 3 * sizeof(_uint);
	MeshDesc.triangles.data = pIndices;
	//MeshDesc.materialIndices = 
	PxDefaultMemoryOutputStream WriteBuffer{};
	if (!PxCookTriangleMesh(CookingParams, MeshDesc, WriteBuffer))
	{
		MSG_BOX("Failed to Cook Mesh");
	}

	PxDefaultMemoryInputData ReadBuffer(WriteBuffer.getData(), WriteBuffer.getSize());
	PxTriangleMesh* pTriangleMesh = m_pPhysics->createTriangleMesh(ReadBuffer);

	PxTriangleMeshGeometry TriangleMeshGeom(pTriangleMesh);
	PxShape* pShape = m_pPhysics->createShape(TriangleMeshGeom, *m_pMaterial);
	pShape->setSimulationFilterData(PxFilterData(COLGROUP_TERRAIN, MASK_TERRAIN, 0, 0));
	PxRigidStatic* pActor = PxCreateStatic(*m_pPhysics, PxTransform(PxIdentity), *pShape);
	m_pScene->addActor(*pActor);
	pShape->release();
#ifdef _DEBUG
	//if (m_pVIBufferCom)
	//{
	//	const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//	m_pVIBufferCom->Update_Buffer(rb);
	//}
	//else
	//{
	//	const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//	int a = rb.getNbLines();
	//	m_pVIBufferCom = CVIBuffer_PhysX::Create(m_pDevice, m_pContext, rb);
	//}
#endif // _DEBUG

	return pActor;
}

CPhysX_Manager* CPhysX_Manager::Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance)
{
	CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init(pGameInstance)))
	{
		MSG_BOX("Failed to Create : CPhysX_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysX_Manager::Free()
{
	m_pMaterial->release();
	m_pDispatcher->release();
	m_pControllerManager->release();
	m_pScene->release();
	m_pPhysics->release();
	m_pFoundation->release();

#ifdef _DEBUG
	Safe_Release(m_pDebugShader);
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
