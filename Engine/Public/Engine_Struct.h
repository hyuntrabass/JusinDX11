#pragma once


namespace Engine
{
	using GRAPHIC_DESC = struct tagGraphic_Desc
	{
		HWND hWnd{};
		HINSTANCE hInst{};
		unsigned int iWinSizeX{}, iWinSizeY{};
		bool isWindowed{};
	};

	using VTXPOSTEX = struct tagVertex_Position_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vTexcoord{};
	};
}