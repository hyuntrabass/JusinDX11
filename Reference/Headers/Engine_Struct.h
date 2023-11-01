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

	using LIGHT_DESC = struct tagLightDesc
	{
		enum TYPE { Directional, Point, End };

		TYPE eType{End};
		XMFLOAT4 vDirection{};
		XMFLOAT4 vPosition{};
		float fRange{};

		XMFLOAT4 vDiffuse{};
		XMFLOAT4 vAmbient{};
		XMFLOAT4 vSpecular{};
	};

	using Model_Material = struct tagMaterial
	{
		class CTexture* pMaterials[18]{};
	};

	using KEYFRAME = struct tagKeyFrame
	{
		float fTime{};
		XMFLOAT4 vScale{};
		XMFLOAT4 vRotation{};
		XMFLOAT4 vPosition{};
	};

	using VTXPOSTEX = struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXNORTEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXCUBETEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Cube
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT3 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXSTATICMESH = struct ENGINE_DLL tagVertex_Static_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		static const unsigned int iNumElements{ 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXANIMMESH = struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		XMUINT4 vBlendIndices{};
		XMFLOAT4 vBlendWeights{};

		static const unsigned int iNumElements{ 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

}