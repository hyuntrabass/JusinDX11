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
	Props,
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
	class CImguiMgr* pImguiMgr{ nullptr };
};

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual ~CImguiMgr() = default;

public:
	void SetPos(const _float4& vPos, class CDummy* pDummy);
	_bool ComputePickPos();

public:
	HRESULT Init(vector<wstring>* Models, vector<string>* pPropCount);
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
	const _char** m_pItemList_Props{ nullptr };
	_int m_iNumProps{};
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
	_bool m_ComputePickPos{};
	_float m_fCamDist{};
	class CDummy* m_pSelectedDummy{ nullptr };

private:
	HRESULT Ready_Layers(vector<wstring>* Models, vector<string>* pPropCount);
	void Create_Dummy(const _int& iListIndex);

	HRESULT Load_Data();
	HRESULT Export_Data();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance, vector<wstring>* Models, vector<string>* pPropCount);
	virtual void Free() override;
};

END