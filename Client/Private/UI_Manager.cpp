#include "UI_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

_uint CUI_Manager::m_iButtonIDs[LEVEL_END]{};

void CUI_Manager::Register_Button(_uint iLevelIndex, const wstring& strButtonTag)
{
    //m_pButtons[iLevelIndex].emplace(strButtonTag, false);
}

void CUI_Manager::Set_ButtonState(_uint iLevelIndex, const wstring& strButtonTag, const _bool& bState)
{
    //auto iter = m_pButtons[iLevelIndex].find(strButtonTag);

    //if (iter == m_pButtons[iLevelIndex].end())
    //{
    //    MSG_BOX("No such Button!");
    //    return;
    //}

    //iter->second = bState;
}

const _bool CUI_Manager::Get_ButtonState(_uint iLevelIndex, const wstring& strButtonTag) const
{
    //auto iter = m_pButtons[iLevelIndex].find(strButtonTag);

    //if (iter == m_pButtons[iLevelIndex].end())
    //{
    //    MSG_BOX("No such Button!");
    //    return false;
    //}

    //return iter->second;
    return false;
}

const _bool CUI_Manager::is_Activated() const
{
    return _bool();
}

HRESULT CUI_Manager::Init(CGameInstance* pGameInstance)
{
    if (!pGameInstance)
    {
        return E_FAIL;
    }

    m_pGameInstance = pGameInstance;

    return S_OK;
}

HRESULT CUI_Manager::Ready_UI_Logo()
{
    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_BackGround"))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_SubTitle"))))
    {
        return E_FAIL;
    }

    ButtonInfo Info{};

    Info.strText = TEXT("새 게임");
    Info.vPos = _float2(g_iWinSizeX >> 1, g_iWinSizeY >> 1);
    Info.fDepth = 0.9f;
    Info.iButtonType = 0;
    Info.iButtonID = m_iButtonIDs[LEVEL_LOGO]++;

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
    {
        return E_FAIL;
    }

    Info.strText = TEXT("불러오기");
    Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 100.f);
    Info.iButtonID = m_iButtonIDs[LEVEL_LOGO]++;

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
    {
        return E_FAIL;
    }

    Info.strText = TEXT("설정");
    Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 200.f);
    Info.iButtonID = m_iButtonIDs[LEVEL_LOGO]++;

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
    {
        return E_FAIL;
    }

    Info.strText = TEXT("게임 종료");
    Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 300.f);
    Info.iButtonID = m_iButtonIDs[LEVEL_LOGO]++;

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CUI_Manager::Ready_UI_Tuto()
{
    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CREATECHARACTER, TEXT("Layer_PartsWindow"), TEXT("Prototype_GameObject_PartsWindow"))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CREATECHARACTER, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_BackGroundCC"))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CREATECHARACTER, TEXT("Layer_Title"), TEXT("Prototype_GameObject_Title_Custom"))))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CUI_Manager::Free()
{
}
