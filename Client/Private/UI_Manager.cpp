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

const _bool CUI_Manager::is_Activated(_uint iIndex) const
{
	return m_Buttons[iIndex]->Is_Activated();
}

const _uint& CUI_Manager::Get_PartIndex(PART_TYPE eType) const
{
	return m_iPartIndex[eType];
}

const _uint& CUI_Manager::Get_PageIndex() const
{
	return m_iPageIndex;
}

const _uint CUI_Manager::Get_sizeofButtons() const
{
	return m_Buttons.size();
}

HRESULT CUI_Manager::Init()
{
	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CUI_Manager::Tick(_float fTimeDelta)
{
	if (m_Buttons.empty())
	{
		return;
	}

	m_Buttons[m_iButtonIndex]->Activate_Button(false);

	_uint iButtons_size = m_Buttons.size();
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CREATECHARACTER and m_iPageIndex != 0)
	{
		iButtons_size--;
	}

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		m_iButtonIndex++;

		if (m_iButtonIndex >= iButtons_size)
		{
			if (m_iNumButtons > iButtons_size && m_iButtonIndex + m_iScroll < m_iNumButtons)
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

			if (m_iButtonIndex >= iButtons_size)
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
				m_iButtonIndex = iButtons_size - 1;
				if (m_iNumButtons > iButtons_size)
				{
					m_iScroll = m_iNumButtons - iButtons_size;
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
				m_iButtonIndex = iButtons_size - 1;
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
		m_pGameInstance->Render_Text(TEXT("Font_Malang"), TEXT("부위 선택"), _float2(400.f, 155.f), 0.6f, Colors::Gold);
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

	Info.strText = TEXT("새 게임");
	Info.vPos = _float2(g_iWinSizeX >> 1, g_iWinSizeY >> 1);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("불러오기");
	Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 100.f);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("설정");
	Info.vPos = _float2(g_iWinSizeX >> 1, (g_iWinSizeY >> 1) + 200.f);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_MainTitle_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);

	Info.strText = TEXT("게임 종료");
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
			Info.strText = TEXT("머리");
			break;
		case 1:
			Info.strText = TEXT("얼굴");
			break;
		case 2:
			Info.strText = TEXT("상의");
			break;
		case 3:
			Info.strText = TEXT("하의");
			break;
		case 4:
			Info.strText = TEXT("한벌 옷");
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
		Safe_AddRef(pButton);

		Info.vPos.y += 50.f;
	}

	Info.vPos = _float2(g_iWinSizeX * 0.5f, 650.f);
	Info.iButtonType = 0;
	Info.strText = TEXT("결정");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CREATECHARACTER, TEXT("Layer_Buttons"), TEXT("Prototype_GameObject_Button_Common"), &Info)))
	{
		return E_FAIL;
	}
	m_Buttons.emplace_back(pButton);
	Safe_AddRef(pButton);

	return S_OK;
}

void CUI_Manager::Customization()
{
	switch (m_iPageIndex)
	{
	case 0: // 부위 고르기
		m_iNumButtons = 7;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("헤어 스타일"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("얼굴"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("상의"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("하의"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("특별 코스튬"));
				break;
			case 5:
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT("결정"));

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
	case 1: // 머리
		m_iNumButtons = 11;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("히맨 스타일"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("보라 단발"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("초코송이 스타일"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("빨강 단발"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("깨구리"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("제빵왕 김탁구"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("사쿠라 스타일"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("자다 깬 스타일"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("긴 생머리 닌자"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("밋밋한 스타일"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("대륙 스타일"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT(""));

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
	case 2: // 얼굴
		m_iNumButtons = 3;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("맨얼굴"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("붕대"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("카카시 마스크"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT(""));

		m_iPartIndex[PT_FACE] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 1;
				break;
			}
		}
		break;
	case 3: // 상의
		m_iNumButtons = 14;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("나뭇잎 의상"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("수영장룩"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("후드티"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("긴팔티"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("쥬신 티셔츠"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("검은 닌자"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("보라 탱크탑1"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("다크 히어로"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("비키니"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("보라 탱크탑2"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("나루토 의상"));
				break;
			case 11:
				m_Buttons[i]->Set_ButtonText(TEXT("네지 의상"));
				break;
			case 12:
				m_Buttons[i]->Set_ButtonText(TEXT("모래닌자 상의"));
				break;
			case 13:
				m_Buttons[i]->Set_ButtonText(TEXT("카카시 의상"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT(""));

		if (m_iPartIndex[PT_UPPER_BODY] >= 14)
		{
			m_iPartIndex[PT_LOWER_BODY] = 0;
		}
		m_iPartIndex[PT_UPPER_BODY] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 2;
				break;
			}
		}
		break;
	case 4: // 하의
		m_iNumButtons = 11;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("트임 치마"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("대륙 바지1"));
				break;
			case 2:
				m_Buttons[i]->Set_ButtonText(TEXT("대륙 바지2"));
				break;
			case 3:
				m_Buttons[i]->Set_ButtonText(TEXT("대륙 바지3"));
				break;
			case 4:
				m_Buttons[i]->Set_ButtonText(TEXT("반바지1"));
				break;
			case 5:
				m_Buttons[i]->Set_ButtonText(TEXT("반바지2"));
				break;
			case 6:
				m_Buttons[i]->Set_ButtonText(TEXT("보라 치마"));
				break;
			case 7:
				m_Buttons[i]->Set_ButtonText(TEXT("모래닌자 하의"));
				break;
			case 8:
				m_Buttons[i]->Set_ButtonText(TEXT("파란 치마"));
				break;
			case 9:
				m_Buttons[i]->Set_ButtonText(TEXT("청록 치마"));
				break;
			case 10:
				m_Buttons[i]->Set_ButtonText(TEXT("파란 치마 + 슬리퍼"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT(""));

		if (m_iPartIndex[PT_UPPER_BODY] >= 14)
		{
			m_iPartIndex[PT_UPPER_BODY] = 0;
		}
		m_iPartIndex[PT_LOWER_BODY] = m_iButtonIndex + m_iScroll;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 3;
				break;
			}
		}
		break;
	case 5: // 한벌 옷
		m_iNumButtons = 2;
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			switch (i + m_iScroll)
			{
			case 0:
				m_Buttons[i]->Set_ButtonText(TEXT("루돌프"));
				break;
			case 1:
				m_Buttons[i]->Set_ButtonText(TEXT("산타걸"));
				break;
			default:
				m_Buttons[i]->Set_ButtonText(TEXT(""));
				break;
			}
		}

		m_Buttons.back()->Set_ButtonText(TEXT(""));

		m_iPartIndex[PT_UPPER_BODY] = m_iButtonIndex + m_iScroll + 14;
		m_iPartIndex[PT_LOWER_BODY] = 100;

		for (auto& pButton : m_Buttons)
		{
			if (pButton->Is_Pushed())
			{
				m_Buttons[m_iButtonIndex]->Activate_Button(false);
				m_iPageIndex = 0;
				m_iScroll = 0;
				m_iButtonIndex = 4;
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
	Safe_Release(m_pGameInstance);
}
