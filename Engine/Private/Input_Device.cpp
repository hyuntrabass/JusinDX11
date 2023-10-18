#include "Input_Device.h"

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Init(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
	{
		return E_FAIL;
	}
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
	{
		return E_FAIL;
	}
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboard->Acquire();

	//if (FAILED(m_pInputSDK->CreateDevice(GUID_Joystick, &m_pGamepad, nullptr)))
	//{
	//	return E_FAIL;
	//}
	//m_pGamepad->SetDataFormat(&c_dfDIJoystick);
	//m_pGamepad->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	//m_pGamepad->Acquire();

	return S_OK;
}

void CInput_Device::Update_InputDev()
{
	m_pMouse->GetDeviceState(sizeof m_MouseState, &m_MouseState);
	m_pKeyboard->GetDeviceState(sizeof m_byKeyState, &m_byKeyState);
	//m_pGamepad->GetDeviceState(sizeof m_GamepadState, &m_GamepadState);
}

_bool CInput_Device::Key_Pressing(_ubyte iKey)
{
	if (m_byKeyState[iKey] & 0x8000)
		return true;

	return false;
}

_bool CInput_Device::Key_Down(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && (m_byKeyState[iKey] & 0x8000))
	{
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = true;
		return true;
	}

	if (m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && !(m_byKeyState[iKey] & 0x8000))
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = false;

	return false;
}

_bool CInput_Device::Key_Up(_ubyte iKey, InputChannel eInputChannel)
{
	if (m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && !(m_byKeyState[iKey] & 0x8000))
	{
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = false;
		return true;
	}

	if (!m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && (m_byKeyState[iKey] & 0x8000))
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = true;


	return false;
}

_long CInput_Device::Get_MouseMove(MouseState eMouseState)
{
	return *(((_long*)&m_MouseState) + ToIndex(eMouseState));
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Init(hInst, hWnd)))
	{
		MSG_BOX("Failed to Create : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free()
{
	Safe_Release(m_pGamepad);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
