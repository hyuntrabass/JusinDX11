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

	enum class LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_EDITOR,
		LEVEL_LOGO,
		LEVEL_CREATECHARACTER,
		LEVEL_TUTORIAL,
		LEVEL_STAGE1,
		LEVEL_STAGE2,
		LEVEL_END
	};

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace MapEditor;