#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

// 외부 라이브러리 헤더
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <fx11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
using namespace DirectX;

#include <Xinput.h>

// 외부 라이브러리 lib
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "dinput8.lib")

#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#else //Release
#pragma comment(lib, "Effects11.lib")
#endif

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <ctime>
#include <fstream>

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

//// 사운드 재생을 위한 헤더
//#include <filesystem>
//#include <io.h>
//#include "fmod.h"
//#include "fmod.hpp"
//#pragma comment(lib, "fmod_vc.lib")

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

namespace Engine
{
	enum class MouseState
	{
		x, y, wheel, end
	};
	enum MOUSE_KEY_STATE
	{
		DIM_LBUTTON,
		DIM_RBUTTON,
		DIM_MBUTTON,
		DIM_END
	};
//#define XINPUT_DPAD_UP          0x0001
//#define XINPUT_DPAD_DOWN        0x0002
//#define XINPUT_DPAD_LEFT        0x0004
//#define XINPUT_DPAD_RIGHT       0x0008
//#define XINPUT_START            0x0010
//#define XINPUT_BACK             0x0020
//#define XINPUT_LEFT_THUMB       0x0040
//#define XINPUT_RIGHT_THUMB      0x0080
//#define XINPUT_LEFT_SHOULDER    0x0100
//#define XINPUT_RIGHT_SHOULDER   0x0200
//#define XINPUT_A                0x1000
//#define XINPUT_B                0x2000
//#define XINPUT_X                0x4000
//#define XINPUT_Y                0x8000

	enum GAMPAD_KEY_STATE
	{
		XINPUT_UP = 0x0001,
		XINPUT_DOWN = 0x0002,
		XINPUT_LEFT = 0x0004,
		XINPUT_RIGHT = 0x0008,
		XINPUT_START = 0x0010,
		XINPUT_BACK = 0x0020,
		XINPUT_LS = 0x0040,
		XINPUT_RS = 0x0080,
		XINPUT_LB = 0x0100,
		XINPUT_RB = 0x0200,
		XINPUT_A = 0x1000,
		XINPUT_B = 0x2000,
		XINPUT_X = 0x4000,
		XINPUT_Y = 0x8000,
		XINPUT_LT,
		XINPUT_RT,
	};

	enum class TextureType
	{
		None,
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normals,
		Shininess,
		Opacity,
		Displacement,
		LightMap,
		Reflection,
		Base_Color,
		Normal_Camera,
		Emission_Color,
		Metalness,
		Diffuse_Roughness,
		Ambient_Occlusion,
		End
	};
}

using namespace std;
using namespace Engine;
