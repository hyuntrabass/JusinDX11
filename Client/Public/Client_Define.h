#pragma once

#include "../Default/framework.h"
#include <process.h>
#include <iostream>
#include "Engine_Defines.h"
#include "Transform.h"

#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "Imm32.lib")

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
		LEVEL_FOREST,
		LEVEL_VILLAGE,
		LEVEL_CLOUD,
		LEVEL_END
	};

	enum CAMERA_MODE
	{
		CM_DEBUG,
		CM_MAIN,
		CM_END
	};

	enum class ItemType
	{
		Map,
		Props,
		Monster,
		NPC,
		End
	};

	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_float4 vPos{_float4(0.f, 0.f, 0.f, 1.f)};
		_float4 vLook{_float4(0.f, 0.f, 1.f, 0.f)};
	};

	struct AttachmentInfo
	{
		CTransform* pParent{ nullptr };
		_float44* pSocket{ nullptr };
		_float44 PivotMatrix{};
	};

	struct ButtonInfo
	{
		wstring strText{};
		_float2 vPos{};
		_uint iButtonType{};
		_float fDepth{};
		class CButton_Common** ppButton{ nullptr };
	};

	enum AnimMeshPass
	{
		AnimPass_Default,
		AnimPass_OutLine,
		AnimPass_Shadow,
		AnimPass_Dissolve,
	};

	enum StaticMeshPass
	{
		StaticPass_Default,
		StaticPass_OutLine,
		StaticPass_AlphaTestMeshes,
		StaticPass_Sky,
		StaticPass_COLMesh,
		StaticPass_SingleColorFx,
		StaticPass_SingleColorDissolve,
		StaticPass_MaskEffect,
		StaticPass_MaskDissolve,
		StaticPass_SingleColoredEffectFrontCull,
		StaticPass_Shadow,
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
		VTPass_HP,
	};

	enum InstancingPass
	{
		InstPass_Particle_Texture,
		InstPass_Particle_Color,
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;