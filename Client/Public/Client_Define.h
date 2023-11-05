#pragma once

#include "../Default/framework.h"
#include <process.h>
#include <iostream>
#include "Engine_Defines.h"

#pragma comment(lib, "Engine.lib")

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static const POINT g_ptCenter = { static_cast<signed long>(g_iWinSizeX * 0.5f), static_cast<signed long>(g_iWinSizeY * 0.5f) };

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_LOGO,
		LEVEL_CREATECHARACTER,
		LEVEL_TUTORIAL,
		LEVEL_STAGE1,
		LEVEL_STAGE2,
		LEVEL_END
	};

	enum class CameraMode
	{
		Debug,
		Main,
		End
	};

	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_float4 vPos{_float4(0.f, 0.f, 0.f, 1.f)};
		_float4 vLook{_float4(0.f, 0.f, 1.f, 0.f)};
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;