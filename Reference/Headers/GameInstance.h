#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev** ppDevice, _Inout_ _context** ppContext);

public:
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: // Timer Manager
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

private:
	class CGraphic_Device* m_pGraphic_Device{ nullptr };

private:
	class CTimer_Manager* m_pTimer_Manager{ nullptr };

public:
	static void Release_Engine();

private:
	void Clear_Managers();

public:
	virtual void Free() override;
};

END