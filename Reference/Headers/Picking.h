#pragma once
#include "Base.h"

BEGIN(Engine);

class CPicking final : public CBase
{
private:
	CPicking(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, class CGameInstance* pGameInstance);
	virtual ~CPicking() = default;

public:
	void Tick();

public:
	const _bool& Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* vPickPos);

private:
	HWND m_hWnd{};
	_uint m_iWinSizeX{}, m_iWinSizeY{};
	class CGameInstance* m_pGameInstance{ nullptr };

	_float4 m_vRayPos_World{};
	_float4 m_vRayDir_World{};

public:
	static CPicking* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, class CGameInstance* pGameInstance);
	virtual void Free() override;
};

END