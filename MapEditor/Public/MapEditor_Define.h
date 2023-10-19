#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "../Default/framework.h"
#include <process.h>

#pragma comment(lib, "Engine.lib")

namespace MapEditor
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum class Level_ID
	{
		Static,
		Editor,
		Logo,
		CreateCharacter,
		Tutorial,
		Stage1,
		Stage2,
		End
	};

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace MapEditor;