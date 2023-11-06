#include "Button_Manager.h"

void CButton_Manager::Register_Button(_uint iLevelIndex, const wstring& strButtonTag)
{
    m_pButtons[iLevelIndex].emplace(strButtonTag, false);
}

void CButton_Manager::Set_ButtonState(_uint iLevelIndex, const wstring& strButtonTag, const _bool& bState)
{
    auto iter = m_pButtons[iLevelIndex].find(strButtonTag);

    if (iter == m_pButtons[iLevelIndex].end())
    {
        MSG_BOX("No such Button!");
        return;
    }

    iter->second = bState;
}

const _bool CButton_Manager::Get_ButtonState(_uint iLevelIndex, const wstring& strButtonTag) const
{
    auto iter = m_pButtons[iLevelIndex].find(strButtonTag);

    if (iter == m_pButtons[iLevelIndex].end())
    {
        MSG_BOX("No such Button!");
        return false;
    }

    return iter->second;
}

HRESULT CButton_Manager::Init(_uint iNumLevel)
{
    m_pButtons = new map<const wstring, _bool>[iNumLevel];

    return S_OK;
}

CButton_Manager* CButton_Manager::Create(_uint iNumLevel)
{
    CButton_Manager* pInstance = new CButton_Manager();

    if (FAILED(pInstance->Init(iNumLevel)))
    {
        MSG_BOX("Failed to Create : CButton_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButton_Manager::Free()
{
    Safe_Delete_Array(m_pButtons);
}
