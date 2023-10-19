#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "../Default/framework.h"
#include <process.h>

#pragma comment(lib, "Engine.lib")

namespace ModelConverter
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace ModelConverter;
using namespace ImGui;