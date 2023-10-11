#pragma once
#include "../Default/framework.h"
#include <process.h>

#pragma comment(lib, "Engine.lib")

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum class Level_ID
	{
		Static,
		Loading,
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