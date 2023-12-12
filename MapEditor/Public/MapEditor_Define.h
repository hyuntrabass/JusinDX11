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

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_EDITOR,
		LEVEL_LOGO,
		LEVEL_CREATECHARACTER,
		LEVEL_FOREST,
		LEVEL_VILLAGE,
		LEVEL_CLOUD,
		LEVEL_END
	};

	enum AnimMeshPass
	{
		AnimPass_Default,
		AnimPass_OutLine,
		AnimPass_Test,
	};

	enum StaticMeshPass
	{
		StaticPass_Default,
		StaticPass_OutLine,
		StaticPass_AlphaTestMeshes,
		StaticPass_Sky,
		StaticPass_COLMesh,
		StaticPass_SingleColorFx,
		StaticPass_MaskEffect,
		SingleColoredEffectFrontCull,
	};

	enum VNTPass
	{
		VNTPass_Terrain,
		VNTPass_Terrain_Editor,
	};

	enum VTPass
	{
		VTPass_UI,
		VTPass_Button,
		VTPass_Background,
		VTPass_Mask_Color,
	};

	enum InstancingPass
	{
		InstPass_Particle_Texture,
		InstPass_Particle_Color,
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace MapEditor;