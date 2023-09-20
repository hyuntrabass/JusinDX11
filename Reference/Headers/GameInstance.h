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
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

private:
	class CTimer_Manager* m_pTimer_Manager{ nullptr };

public:
	void Release_Managers();
	static void Release_Engine();

public:
	virtual void Free() override;
};

END