#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"

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

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);

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

void CGameInstance::Clear_Managers()
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Clear_Managers();

	CGameInstance::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pGraphic_Device);
}
