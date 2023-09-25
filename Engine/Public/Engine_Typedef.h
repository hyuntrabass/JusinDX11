#pragma once


namespace Engine
{
	/* 프로그램내에 사용되는 여러 타입들을 재정의한다.  */
	using _char = char;
	using _tchar = wchar_t;

	using _ubyte = unsigned char;
	using _byte = signed char;

	using _ushort = unsigned short;
	using _short = signed short;

	using _uint = unsigned int;
	using _int = signed int;

	using _ulong = unsigned long;
	using _long = signed long;

	using _float = float;
	using _double = double;

	using _bool = bool;

	using _lint = LARGE_INTEGER;

	using _float2 = XMFLOAT2;
	using _float3 = XMFLOAT3;
	using _float4 = XMFLOAT4;
	using _vector = XMVECTOR;

	using _float4x4 = XMFLOAT4X4;
	using _matrix = XMMATRIX;

	using _dev = ID3D11Device*;
	using _context = ID3D11DeviceContext*;
}
