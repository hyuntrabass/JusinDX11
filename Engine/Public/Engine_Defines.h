#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

// �ܺ� ���̺귯�� ���
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
using namespace DirectX;

// �ܺ� ���̺귯�� lib
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

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

//// ���� ����� ���� ���
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

using namespace std;
using namespace Engine;
