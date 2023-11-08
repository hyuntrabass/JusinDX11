#include "PhysX.h"
#include "Transform.h"
#include "VIBuffer_PhysX.h"
#include "Shader.h"
#include "GameInstance.h"

CPhysX::CPhysX(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysX::Init(CGameInstance* pGameInstance)
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

void CPhysX::Tick(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);
}

#ifdef _DEBUG
HRESULT CPhysX::Render()
{
	if (!m_pVIBufferCom)
	{
		return S_OK;
	}
	else
	{
		const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
		m_pVIBufferCom->Update_Buffer(rb);
	}

	if (FAILED(m_pDebugShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDebugShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDebugShader->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}
#endif // _DEBUG

void CPhysX::Init_Dynamic_PhysX(CTransform* pTransform)
{
	_float44 World = pTransform->Get_World_float4x4();
	PxMat44 WorldMat = PxMat44(reinterpret_cast<_float*>(&World));
	PxTransform Pose = PxTransform(WorldMat);

	PxShape* pShape = m_pPhysics->createShape(PxCapsuleGeometry(0.3f, 1.8f), *m_pMaterial);
	PxRigidDynamic* pActor = m_pPhysics->createRigidDynamic(Pose);
	pActor->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*pActor, 10.f);
	pActor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
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

void CPhysX::Init_Static_PhysX(CTransform* pTransform)
{
}

void CPhysX::Apply_PhysX(CTransform* pTransform)
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

void CPhysX::Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices)
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
	PxRigidStatic* pActor = PxCreateStatic(*m_pPhysics, PxTransform(PxIdentity), *pShape);
	m_pScene->addActor(*pActor);

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

}

CPhysX* CPhysX::Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance)
{
	CPhysX* pInstance = new CPhysX(pDevice, pContext);

	if (FAILED(pInstance->Init(pGameInstance)))
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

	Safe_Release(m_pDebugShader);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
