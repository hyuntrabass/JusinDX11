#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

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

using namespace std;
using namespace Engine;
