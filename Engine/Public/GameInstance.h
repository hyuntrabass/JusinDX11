#pragma once
#include "Base.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "PhysX_Manager.h"
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev* ppDevice, _Inout_ _context* ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: // Graphic Device
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: // Timer Manager
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: // Level Manager
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	HRESULT Render();

public: // Object Manager
	HRESULT Add_Prototype_GameObejct(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);

public: // Component Manager
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const wstring& strPrototype, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: // Input Manager
	_bool Key_Pressing(_ubyte iKey);
	_bool Key_Down(_ubyte iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Key_Up(_ubyte iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Mouse_Pressing(_long iKey);
	_bool Mouse_Down(_long iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Mouse_Up(_long iKey, InputChannel eInputChannel = InputChannel::Default);
	_long Get_MouseMove(MouseState eMouseState);
	_bool Gamepad_Pressing(GAMPAD_KEY_STATE eKey);
	_bool Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel = InputChannel::Default);
	// Trigger pushed amount between 0 and 1
	const _float& Gamepad_LTrigger() const;
	// Trigger pushed amount between 0 and 1
	const _float& Gamepad_RTrigger() const;
	// Stick coord between 0 and 1
	const _float2& Gamepad_LStick() const;
	// Stick coord between 0 and 1
	const _float2& Gamepad_RStick() const;
	void Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed);

public: // Light Manager
	HRESULT Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iLevelIndex, _uint iIndex) const;
	HRESULT Render_Lights(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: // PipeLine
	_float4 Get_CameraPos() const;
	_float4 Get_CameraLook() const;
	_float44 Get_Transform_Float4x4(D3DTS eState) const;
	_float44 Get_Transform_Inversed_Float4x4(D3DTS eState) const;
	_matrix Get_Transform(D3DTS eState) const;
	_matrix Get_Transform_Inversed(D3DTS eState) const;

	void Set_Transform(D3DTS eState, const _float44& TransformMatrix);
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);

public: // Picking
	void TransformRay_ToLocal(_fmatrix WorldMatrix);

	_bool Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float4* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _gvector vNormal, _Inout_ _float4* pPickPos);

public: // Font
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFilePath);
	HRESULT Render_Text(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _fvector vColor = Colors::White, _float fRotation = 0.f);

public: // Frustum
	_bool IsIn_Fov_World(_fvector vPos, _float fRange = 0.f);
	void Transform_ToLocalSpace(_fmatrix vWorldInversed);
	_bool IsIn_Fov_Local(_fvector vPos, _float fRange = 0.f);

public: // Collision
	HRESULT Register_CollisionObject(class CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer = false);
	void Delete_CollisionObject(class CGameObject* pObject, _bool IsPlayer = false);
	void Attack_Monster(class CCollider* pCollider, _uint iDamage);
	void Attack_Player(class CCollider* pCollider, _uint iDamage);

public: // PhysX
	void Init_PhysX_Character(class CTransform* pTransform, CollisionGroup eGroup, PxCapsuleControllerDesc* pDesc = nullptr);
	void Init_PhysX_MoveableObject(class CTransform* pTransform);
	void Apply_PhysX(class CTransform* pTransform);
	void Update_PhysX(class CTransform* pTransform);
	PxRigidStatic* Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices);
	_bool Raycast(_float3 vOrigin, _float3 vDir, _float fDist, PxRaycastBuffer& Buffer);
	void PhysXTick(_float fTimeDelta);
#ifdef _DEBUG
	HRESULT Render_PhysX();
#endif // _DEBUG

public: // RenderTarget
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pVariableName, const wstring& strTargetTag);
#ifdef _DEBUG
public:
	HRESULT Ready_Debug_RT(const wstring& strTargetTag, _float2 vPos, _float2 vSize);
	HRESULT Render_Debug_RT(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

public: // Get_Set
	const _uint& Get_CameraModeIndex() const;
	const _uint& Get_CurrentLevelIndex() const;
	const _float& Get_TimeRatio() const;

	void Set_CameraModeIndex(const _uint& iIndex);
	void Set_CurrentLevelIndex(const _uint& iIndex);
	void Set_TimeRatio(const _float fRatio);

private:
	class CGraphic_Device* m_pGraphic_Device{ nullptr };

private:
	class CTimer_Manager* m_pTimer_Manager{ nullptr };
	class CLevel_Manager* m_pLevel_Manager{ nullptr };
	class CObject_Manager* m_pObject_Manager{ nullptr };
	class CComponent_Manager* m_pComponent_Manager{ nullptr };
	class CInput_Device* m_pInput_Manager{ nullptr };
	class CLight_Manager* m_pLight_Manager{ nullptr };
	class CFont_Manager* m_pFont_Manager{ nullptr };
	class CFrustum* m_pFrustum{ nullptr };
	class CCollision_Manager* m_pCollision_Manager{ nullptr };
	class CRenderTarget_Manager* m_pRenderTarget_Manager{ nullptr };

	class CPipeLine* m_pPipeLine{ nullptr };
	class CPicking* m_pPicking{ nullptr };
	class CPhysX_Manager* m_pPhysX_Manager{ nullptr };

private:
	_uint m_iCameraModeIndex{};
	_uint m_iLevelIndex{};
	_float m_fTimeRatio{ 1.f };

public:
	static void Release_Engine();

private:
	void Clear_Managers();

public:
	virtual void Free() override;
};

END