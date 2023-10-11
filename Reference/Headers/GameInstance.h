#pragma once
#include "Base.h"
#include "Component_Manager.h"
#include "PipeLine.h"

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
	const _uint& Get_CurrentLevelIndex() const;

	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	HRESULT Render();

public: // Object Manager
	HRESULT Add_Prototype_GameObejct(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);

public: // Component Manager
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const wstring& strPrototype, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: // PipeLine
	_float4 Get_CameraPos() const;
	_float4x4 Get_Transform_Float4x4(D3DTS eState) const;
	_float4x4 Get_Transform_Inversed_Float4x4(D3DTS eState) const;
	_matrix Get_Transform(D3DTS eState) const;
	_matrix Get_Transform_Inversed(D3DTS eState) const;

	void Set_Transform(D3DTS eState, const _float4x4& TransformMatrix);
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);

public: // Camera Mode
	const _uint& Get_CameraModeIndex() const;

	void Set_CameraModeIndex(const _uint& iIndex);

private:
	class CGraphic_Device* m_pGraphic_Device{ nullptr };

private:
	class CTimer_Manager* m_pTimer_Manager{ nullptr };
	class CLevel_Manager* m_pLevel_Manager{ nullptr };
	class CObject_Manager* m_pObject_Manager{ nullptr };
	class CComponent_Manager* m_pComponent_Manager{ nullptr };

	class CPipeLine* m_pPipeLine{ nullptr };

private:
	_uint m_iCameraModeIndex{};

public:
	static void Release_Engine();

private:
	void Clear_Managers();

public:
	virtual void Free() override;
};

END