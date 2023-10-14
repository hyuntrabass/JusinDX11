#pragma once

#include "../Default/framework.h"
#include <process.h>
#include <iostream>

#pragma comment(lib, "Engine.lib")

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static const POINT g_ptCenter = { static_cast<signed long>(g_iWinSizeX * 0.5f), static_cast<signed long>(g_iWinSizeY * 0.5f) };

	enum class Level_ID
	{
		Static,
		Loading,
		Editor,
		Logo,
		CreateCharacter,
		Tutorial,
		Stage1,
		Stage2,
		End
	};

	enum class CameraMode
	{
		Debug,
		End
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;