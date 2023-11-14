#include "UI_Manager.h"
#include "GameInstance.h"
#include "Button_Common.h"

IMPLEMENT_SINGLETON(CUI_Manager)

void CUI_Manager::Set_ButtonState(const wstring& strButtonTag, const _bool& bState)
{
	//auto iter = m_Buttons[iLevelIndex].find(strButtonTag);

	//if (iter == m_Buttons[iLevelIndex].end())
	//{
	//    MSG_BOX("No such Button!");
	//    return;
	//}

	//iter->second = bState;
}

const _bool CUI_Manager::Is_ButtonPushed(_uint iIndex) const
{
	return m_Buttons[iIndex]->Is_Pushed();
}

const _bool CUI_Manager::is_Activated() const
{
	return _bool();
}

const _uint& CUI_Manager::Get_PartIndex(PART_TYPE eType)
{
	return m_iPartIndex[eType];
}

const _uint& CUI_Manager::Get_PageIndex()
{
	return m_iPageIndex;
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

void CUI_Manager::Tick(_float fTimeDelta)
{
	if (m_Buttons.empty())
	{
		return;
	}

	m_Buttons[m_iButtonIndex]->Activate_Button(false);

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		m_iButtonIndex++;

		if (m_iButtonIndex >= m_Buttons.size())
		{
			if (m_iNumButtons > m_Buttons.size() && m_iButtonIndex + m_iScroll < m_iNumButtons)
			{
				m_iScroll++;
				m_iButtonIndex--;
			}
			else
			{
				m_iScroll = 0;
				m_iButtonIndex = 0;
			}
		}

		while (m_Buttons[m_iButtonIndex]->Is_Empty())
		{
			m_iButtonIndex++;

			if (m_iButtonIndex >= m_Buttons.size())
			{
				m_iButtonIndex = 0;
			}
		}
	}

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iButtonIndex == 0)
		{
			if (m_iScroll)
			{
				m_iScroll--;
			}
			else
			{
				m_iButtonIndex = m_Buttons.size() - 1;
				if (m_iNumButtons > m_Buttons.size())
				{
					m_iScroll = m_iNumButtons - m_Buttons.size();
				}
			}
		}
		else
		{
			m_iButtonIndex--;
		}

		while (m_Buttons[m_iButtonIndex]->Is_Empty())
		{
			if (m_iButtonIndex == 0)
			{
				m_iScroll = m_iNumButtons - m_Buttons.size();
				m_iButtonIndex = m_Buttons.size() - 1;
			}
			else
			{
				m_iButtonIndex--;
			}
		}
	}

	m_Buttons[m_iButtonIndex]->Activate_Button(true);

	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_Buttons[m_iButtonIndex]->Push(true);
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CREATECHARACTER)
	{
		Customization();
	}
}

void CUI_Manager::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI_Manager::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CREATECHARACTER)
	{
		m_pGameInstance->Render_Text(TEXT("Font_Malang"), TEXT("���� ����"), _float2(400.f, 155.f), 0.6f, Colors::Gold);
	}

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
	CButton_Common* pButton{ nullptr };
	Info.fDepth = 0.9f;
	Info.iButtonType = 0;
	Info.ppButton = &pButton;

	Info.strText = TEXT("�� ����");
	Info.vPos = _float2(g_iWinSizeX >> 1, g_iWinSizeY >> 1);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("�ҷ�����");
	Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 100.f);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("����");
	Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 200.f);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("���� ����");
	Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 300.f);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	m_iNumButtons = 4;

	return S_OK;
}

HRESULT CUI_Manager::Ready_UI_Custom()
{
	m_Buttons.clear();

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

	ButtonInfo Info{};
	CButton_Common* pButton{ nullptr };
	Info.fDepth = 0.5f;
	Info.iButtonType = 1;
	Info.ppButton = &pButton;

	Info.vPos = _float2(400.f, 220.f);

	for (size_t i = 0; i < 7; i++)
	{
		switch (i)
		{
		case 0:
			Info.strText = TEXT("�Ӹ�");
			break;
		case 1:
			Info.strText = TEXT("��");
			break;
		case 2:
			Info.strText = TEXT("����");
			break;
		case 3:
			Info.strText = TEXT("����");
			break;
		case 4:
			Info.strText = TEXT("�ѹ� ��");
			break;
		case 5:
		case 6:
			Info.strText = TEXT("");
			break;
		}

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CREATECHARACTER, TEXT("Layer_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
		{
			return E_FAIL;
		}
		m_Buttons.emplace_back(pButton);

		Info.vPos.y += 50.f;
	}

	return S_OK;
}

void CUI_Manager::Customization()
{
	switch (m_iPageIndex)
	{
	case 0: // ���� ����
		m_iNumButtons = 7;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("��"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("����"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("����"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("�ѹ� ��"));
				break;
			case 5:
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		if (m_Buttons[0]->Is_Pushed())
		{
			m_Buttons[m_iButtonIndex]->Activate_Button(false);
			m_iScroll = 0;
			m_iButtonIndex = 0;
			m_iPageIndex = 1;
		}
		else if (m_Buttons[1]->Is_Pushed())
		{
			m_Buttons[m_iButtonIndex]->Activate_Button(false);
			m_iScroll = 0;
			m_iButtonIndex = 0;
			m_iPageIndex = 2;
		}
		else if (m_Buttons[2]->Is_Pushed())
		{
			m_Buttons[m_iButtonIndex]->Activate_Button(false);
			m_iScroll = 0;
			m_iButtonIndex = 0;
			m_iPageIndex = 3;
		}
		else if (m_Buttons[3]->Is_Pushed())
		{
			m_Buttons[m_iButtonIndex]->Activate_Button(false);
			m_iScroll = 0;
			m_iButtonIndex = 0;
			m_iPageIndex = 4;
		}
		else if (m_Buttons[4]->Is_Pushed())
		{
			m_Buttons[m_iButtonIndex]->Activate_Button(false);
			m_iScroll = 0;
			m_iButtonIndex = 0;
			m_iPageIndex = 5;
		}
		break;
	case 1: // �Ӹ�
		m_iNumButtons = 11;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�1"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�2"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�3"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�4"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�5"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�6"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�7"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�8"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�9"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�10"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ӹ�11"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_iPartIndex[PT_HEAD] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 0;
				break;
			}
		}
		break;
	case 2: // ��
		m_iNumButtons = 4;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("�Ǿ�"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("�ش�"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("����ũ1"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("����ũ2"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_iPartIndex[PT_FACE] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 0;
				break;
			}
		}
		break;
	case 3: // ����
		m_iNumButtons = 14;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("����1"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("����2"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("����3"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("����4"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("����5"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("����6"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("����7"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("����8"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("����9"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("����10"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("����11"));
				break;
			case 11:
				m_Buttons[i]->Set_ButtonText(TEXT("����12"));
				break;
			case 12:
				m_Buttons[i]->Set_ButtonText(TEXT("����13"));
				break;
			case 13:
				m_Buttons[i]->Set_ButtonText(TEXT("����14"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_iPartIndex[PT_UPPER_BODY] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 0;
				break;
			}
		}
		break;
	case 4: // ����
		m_iNumButtons = 11;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("����1"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("����2"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("����3"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("����4"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("����5"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("����6"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("����7"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("����8"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("����9"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("����10"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("����11"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_iPartIndex[PT_LOWER_BODY] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 0;
				break;
			}
		}
		break;
	case 5: // �ѹ� ��
		m_iNumButtons = 2;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("�絹��"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("��Ÿ��"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_iPartIndex[PT_UPPER_BODY] = m_iButtonIndex + m_iScroll + 14;
		m_iPartIndex[PT_LOWER_BODY] = 100;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 0;
				break;
			}
		}
		break;
	}

}

void CUI_Manager::Clear_Buttons()
{
	for (auto& pButton : m_Buttons)
	{
		Safe_Release(pButton);
	}
	m_Buttons.clear();
}

void CUI_Manager::Free()
{
	Clear_Buttons();
}
