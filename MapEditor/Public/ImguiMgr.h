#pragma once
#include "MapEditor_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MapEditor)

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual ~CImguiMgr() = default;

public:
	HRESULT Init();
	HRESULT DrawEditor();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance { nullptr };

private: // for Input
	_int m_Curr_Stage{};
	_bool m_bDemo{};
	_float4 m_pPos{0.f, 0.f, 0.f, 1.f};
	_float4 m_pLook{0.f, 0.f, 1.f, 0.f};
	_int m_Curr_Misc{};
	const _char* const m_pItemList_Misc[3]
	{
		"Box",
		"Tree",
		"Rock",
	};
	_int m_Curr_Monster{};
	const _char* const m_pItemList_Monster[3]
	{
		"Leaf_Ninja",
		"Cloud_Ninja",
		"Sand_Ninja",
	};
	_int m_Curr_NPC{};
	const _char* const m_pItemList_NPC[3]
	{
		"Kakashi",
		"Naruto",
		"Sakura",
	};

private:
	void Tick();
	HRESULT Ready_Layers();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual void Free() override;
};

END