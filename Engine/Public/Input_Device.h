#pragma once
#ifndef InputDev_h__
#define InputDev_h__
#endif // !InputDev_h__

#include "Base.h"

BEGIN(Engine)

enum class InputChannel
{
	Default,
	UI,
	GamePlay,
	Editor,
	End
};

enum class MouseState
{
	x, y, wheel, end
};

class CInput_Device final : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Update_InputDev();

public:
	_bool Key_Pressing(_ubyte iKey);
	_bool Key_Down(_ubyte iKey, InputChannel eInputChannel);
	_bool Key_Up(_ubyte iKey, InputChannel eInputChannel);
	_long Get_MouseMove(MouseState eMouseState);

private:
	LPDIRECTINPUT8 m_pInputSDK{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyboard{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pGamepad{ nullptr };

private:
	_bool m_bPrevFrame_KeyState[ToIndex(InputChannel::End)][0xff]{};
	_byte m_byKeyState[0xff]{};
	DIMOUSESTATE m_MouseState{};
	DIJOYSTATE m_GamepadState{};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free() override;
};

END