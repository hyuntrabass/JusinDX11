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

	enum DAMAGE_TYPE
	{
		DAM_NORMAL,
		DAM_ELECTRIC,
		DAM_FIRE,
		DAM_PUSH,
		DAM_COMBO_BEGIN,
		DAM_COMBO_MIDDLE,
		DAM_COMBO_END,
		DAM_END
	};
	
	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_float4 vPos{_float4(0.f, 0.f, 0.f, 1.f)};
		_float4 vLook{_float4(0.f, 0.f, 1.f, 0.f)};
	};

	struct EffectInfo
	{
		_float4 vColor{};
		_float fScale{};
		_float4 vPos{};
		_uint iType{};
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
		StaticPass_NonLight,
		StaticPass_OutLine,
		StaticPass_AlphaTestMeshes,
		StaticPass_Sky,
		StaticPass_COLMesh,
		StaticPass_SingleColorFx,
		StaticPass_SingleColorDissolve,
		StaticPass_Fireball,
		StaticPass_MaskEffect,
		StaticPass_MaskDissolve,
		StaticPass_MaskEffectClamp,
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
		VTPass_Mask_Texture,
		VTPass_Inv_Mask_Texture,
		VTPass_Mask_Color,
		VTPass_Mask_ColorAlpha,
		VTPass_HP,
		VTPass_Hit,
		VTPass_Sprite,
		VTPass_Hell,
	};

	enum InstancingPass
	{
		InstPass_Particle_TextureMask,
		InstPass_Particle_Sprite,
		InstPass_Particle_MaskColor,
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;