#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Picking.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Frustum.h"
#include "Collision_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev* ppDevice, _Inout_ _context* ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (!m_pGraphic_Device)
	{
		return E_FAIL;
	}

	m_pTimer_Manager = CTimer_Manager::Create();
	if (!m_pTimer_Manager)
	{
		return E_FAIL;
	}

	m_pLevel_Manager = CLevel_Manager::Create();
	if (!m_pLevel_Manager)
	{
		return E_FAIL;
	}

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (!m_pObject_Manager)
	{
		return E_FAIL;
	}

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels, *ppDevice, *ppContext);
	if (!m_pComponent_Manager)
	{
		return E_FAIL;
	}

	m_pInput_Manager = CInput_Device::Create(GraphicDesc.hInst, GraphicDesc.hWnd);
	if (!m_pInput_Manager)
	{
		return E_FAIL;
	}

	m_pLight_Manager = CLight_Manager::Create(iNumLevels);
	if (!m_pLight_Manager)
	{
		return E_FAIL;
	}

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (!m_pFont_Manager)
	{
		return E_FAIL;
	}

	m_pFrustum = CFrustum::Create();
	if (!m_pFrustum)
	{
		return E_FAIL;
	}

	m_pCollision_Manager = CCollision_Manager::Create();
	if (!m_pFrustum)
	{
		return E_FAIL;
	}

	m_pPipeLine = CPipeLine::Create();
	if (!m_pPipeLine)
	{
		return E_FAIL;
	}

	m_pPicking = CPicking::Create(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY);
	if (!m_pPicking)
	{
		return E_FAIL;
	}

	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext);
	if (!m_pPhysX_Manager)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	m_pInput_Manager->Update_InputDev();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pPipeLine->Tick();
	m_pPicking->Tick();
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pFrustum->Tick();

	m_pObject_Manager->Release_DeadObjects();
	m_pObject_Manager->Late_Tick(fTimeDelta);

}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}

	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pTimer_Manager is NULL");
	}

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pTimer_Manager is NULL");
	}

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNextLevel);
}

HRESULT CGameInstance::Render()
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Add_Prototype_GameObejct(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Add_Layer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype_Component(_uint iLevelIndex, const wstring& strPrototype, CComponent* pPrototype)
{
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototype, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

_bool CGameInstance::Key_Pressing(_ubyte iKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Pressing(iKey);
}

_bool CGameInstance::Key_Down(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Down(iKey, eInputChannel);
}

_bool CGameInstance::Key_Up(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Up(iKey, eInputChannel);
}

_bool CGameInstance::Mouse_Pressing(_long iKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Pressing(iKey);
}

_bool CGameInstance::Mouse_Down(_long iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Down(iKey, eInputChannel);
}

_bool CGameInstance::Mouse_Up(_long iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Up(iKey, eInputChannel);
}

_long CGameInstance::Get_MouseMove(MouseState eMouseState)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Get_MouseMove(eMouseState);
}

_bool CGameInstance::Gamepad_Pressing(GAMPAD_KEY_STATE eKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Pressing(eKey);
}

_bool CGameInstance::Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Down(eKey, eInputChannel);
}

_bool CGameInstance::Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Up(eKey, eInputChannel);
}

const _float& CGameInstance::Gamepad_LTrigger() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_LTrigger();
}

const _float& CGameInstance::Gamepad_RTrigger() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_RTrigger();
}

const _float2& CGameInstance::Gamepad_LStick() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_LStick();
}

const _float2& CGameInstance::Gamepad_RStick() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_RStick();
}

void CGameInstance::Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	m_pInput_Manager->Gamepad_Vibration(LMotorSpeed, RMotorSpeed);
}

HRESULT CGameInstance::Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Add_Light(iLevelIndex, LightDesc);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iLevelIndex, _uint iIndex) const
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Get_LightDesc(iLevelIndex, iIndex);
}

_float4 CGameInstance::Get_CameraPos() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_CameraPos();
}

_float4 CGameInstance::Get_CameraLook() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_CameraLook();
}

_float44 CGameInstance::Get_Transform_Float4x4(D3DTS eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_float44 CGameInstance::Get_Transform_Inversed_Float4x4(D3DTS eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform_Inversed_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform(D3DTS eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform(eState);
}

_matrix CGameInstance::Get_Transform_Inversed(D3DTS eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform_Inversed(eState);
}

void CGameInstance::Set_Transform(D3DTS eState, const _float44& TransformMatrix)
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::TransformRay_ToLocal(_fmatrix WorldMatrix)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->TransformRay_ToLocal(WorldMatrix);
}

_bool CGameInstance::Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _float3* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InWorld(vPoint1, vPoint2, vPoint3, pPickPos);
}

_bool CGameInstance::Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _float4* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InLocal(vPoint1, vPoint2, vPoint3, pPickPos);
}

_bool CGameInstance::Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _gvector vNormal, _float4* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InLocal(vPoint1, vPoint2, vPoint3, vNormal, pPickPos);
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFilePath)
{
	if (!m_pFont_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pFont_Manager is NULL");
	}

	return m_pFont_Manager->Add_Font(strFontTag, strFilePath);
}

HRESULT CGameInstance::Render_Text(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation)
{
	if (!m_pFont_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pFont_Manager is NULL");
	}

	return m_pFont_Manager->Render(strFontTag, strText, vPosition, fScale, vColor, fRotation);
}

_bool CGameInstance::IsIn_Fov_World(_fvector vPos, _float fRange)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	return m_pFrustum->IsIn_Fov_World(vPos, fRange);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix vWorldInversed)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	m_pFrustum->Transform_ToLocalSpace(vWorldInversed);
}

_bool CGameInstance::IsIn_Fov_Local(_fvector vPos, _float fRange)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	return m_pFrustum->IsIn_Fov_Local(vPos, fRange);
}

HRESULT CGameInstance::Register_CollisionObject(CGameObject* pObject, CCollider* pHitCollider, _bool IsPlayer)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Register_CollisionObject(pObject, pHitCollider, IsPlayer);
}

void CGameInstance::Delete_CollisionObject(CGameObject* pObject, _bool IsPlayer)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	m_pCollision_Manager->Delete_CollisionObject(pObject, IsPlayer);
}

void CGameInstance::Attack_Monster(CCollider* pCollider, _uint iDamage)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Attack_Monster(pCollider, iDamage);
}

void CGameInstance::Attack_Player(CCollider* pCollider, _uint iDamage)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Attack_Player(pCollider, iDamage);
}

void CGameInstance::Init_PhysX_Character(CTransform* pTransform, CollisionGroup eGroup, PxCapsuleControllerDesc* pDesc)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Init_PhysX_Character(pTransform, eGroup, pDesc);
}

void CGameInstance::Init_PhysX_MoveableObject(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Init_PhysX_MoveableObject(pTransform);
}

void CGameInstance::Apply_PhysX(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Apply_PhysX(pTransform);
}

void CGameInstance::Update_PhysX(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Update_PhysX(pTransform);
}

PxRigidStatic* CGameInstance::Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Cook_StaticMesh(iNumVertices, pVertices, iNumIndices, pIndices);
}

_bool CGameInstance::Raycast(_float3 vOrigin, _float3 vDir, _float fDist, _float3* pCollidedPos)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Raycast(vOrigin, vDir, fDist, pCollidedPos);
}

void CGameInstance::PhysXTick(_float fTimeDelta)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Tick(fTimeDelta);
}

#ifdef _DEBUG
HRESULT CGameInstance::Render_PhysX()
{
	if (FAILED(m_pPhysX_Manager->Render()))
	{
		MSG_BOX("Failed to Render PhysX");
		return E_FAIL;
	}

	return S_OK;
}
#endif // _DEBUG

const _uint& CGameInstance::Get_CameraModeIndex() const
{
	return m_iCameraModeIndex;
}

const _uint& CGameInstance::Get_CurrentLevelIndex() const
{
	return m_iLevelIndex;
}

const _float& CGameInstance::Get_TimeRatio() const
{
	return m_fTimeRatio;
}

void CGameInstance::Set_CameraModeIndex(const _uint& iIndex)
{
	m_iCameraModeIndex = iIndex;
}

void CGameInstance::Set_CurrentLevelIndex(const _uint& iIndex)
{
	m_iLevelIndex = iIndex;
}

void CGameInstance::Set_TimeRatio(const _float fRatio)
{
	m_fTimeRatio = fRatio;
}

void CGameInstance::Clear_Managers()
{
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pFrustum);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Clear_Managers();

	CGameInstance::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pGraphic_Device);
}
