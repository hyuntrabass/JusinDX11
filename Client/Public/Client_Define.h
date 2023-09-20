#pragma once
#include "../Default/framework.h"

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum class Level_ID
	{
		Static,
		Loading,
		Logo,
		GamePlay,
		End
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;