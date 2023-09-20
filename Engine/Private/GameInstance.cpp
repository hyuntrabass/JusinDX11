#include "GameInstance.h"
#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev** ppDevice, _Inout_ _context** ppContext)
{
	m_pTimer_Manager = CTimer_Manager::Create();
	if (!m_pTimer_Manager)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		return E_FAIL;
	}

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		return 0.f;
	}

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Release_Managers()
{
	Safe_Release(m_pTimer_Manager);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Release_Managers();

	CGameInstance::Destroy_Instance();
}

void CGameInstance::Free()
{
}
