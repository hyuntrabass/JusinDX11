#pragma once
#include "MapEditor_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MapEditor)

enum class ItemType
{
	Map,
	Misc,
	Monster,
	NPC,
	End
};

struct DummyInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iIndex{};
	_uint iStageIndex{};
	_float4 vPos{};
	_float4 vLook{};
};

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual ~CImguiMgr() = default;

public:
	HRESULT Init(vector<wstring>* Models);
	void Tick();
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance { nullptr };

private: // for Input
	_int m_Curr_Stage{};
	ItemType m_eItemType{ ItemType::End };
	_float4 m_pPos{0.f, 0.f, 0.f, 1.f};
	_float4 m_pLook{0.f, 0.f, 1.f, 0.f};
	const _char* const m_pItemList_Map[19]
	{
		"Konohavill_Ground_01",
		"Konohavill_Ground_02",
		"Konohavill_Ground_03",
		"Konohavill_Ground_04",
		"Konohavill_Ground_05",
		"Konohavill_Ground_06",
		"Konohavill_Ground_07",
		"Konohavill_Ground_08",
		"Konohavill_Ground_09",
		"Konohavill_Ground_10",
		"Konohavill_Ground_11",
		"Konohavill_Ground_12",
		"Konohavill_Ground_13",
		"Konohavill_Ground_14",
		"Konohavill_Building_A",
		"Tower",
		"AreaH_Building_A_1",
		"AreaH_Building_A_2",
		"AreaH_Building_A_3",
	};
	const _char* const m_pItemList_Misc[3]
	{
		"Box",
		"Tree",
		"Rock",
	};
	const _char* const m_pItemList_Monster[3]
	{
		"Leaf_Ninja",
		"Cloud_Ninja",
		"Sand_Ninja",
	};
	const _char* const m_pItemList_NPC[3]
	{
		"Kakashi",
		"Naruto",
		"Sakura",
	};

private:
	list<DummyInfo> m_DummyList{};

private:
	HRESULT Ready_Layers(vector<wstring>* Models);
	void Create_Dummy(const _int& iListIndex);

	HRESULT Load_Data();
	HRESULT Export_Data();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance, vector<wstring>* Models);
	virtual void Free() override;
};

END