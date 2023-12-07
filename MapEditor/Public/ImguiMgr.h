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
	COLMap,
	End
};

struct DummyInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_float4 vPos{};
	_float4 vLook{};
	class CImguiMgr* pImguiMgr{ nullptr };
	class CDummy** ppDummy{ nullptr };
};

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual ~CImguiMgr() = default;

public:
	void SetPos(const _float4& vPos, class CDummy* pDummy);
	void Select(const _float4& vPos, class CDummy* pDummy);
	_bool ComputePickPos();
	_bool ComputeSelection();

public:
	HRESULT Init(vector<wstring>* MapList, vector<wstring>* MapCOLList, vector<string>* PropList);
	void Tick();
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance { nullptr };

private: // for Input
	_int m_Curr_Stage{};
	ItemType m_eItemType{ ItemType::End };
	_float4 m_vPos{0.f, 0.f, 0.f, 1.f};
	_float4 m_vLook{0.f, 0.f, 1.f, 0.f};
	const _char** m_pItemList_Props{ nullptr };
	_int m_iNumProps{};
	const _char* const m_pItemList_Monster[2]
	{
		"Kurama",
		"Pain",
	};
	const _char* const m_pItemList_NPC[3]
	{
		"Kakashi",
		"Naruto",
		"Sakura",
	};

private:
	list<class CDummy*> m_DummyList{};
	_bool m_ComputePickPos{};
	_bool m_ComputeSelection{};
	_float m_fCamDist{};
	class CDummy* m_pSelectedDummy{ nullptr };
	vector<wstring>* m_pMapModels{ nullptr };
	vector<wstring>* m_pMapCOLModels{ nullptr };

private:
	HRESULT Ready_Layers(vector<string>* pPropCount);
	void Create_Dummy(const _int& iListIndex);
	void Delete_Dummy();

	HRESULT Load_Data();
	HRESULT Export_Data();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance, vector<wstring>* MapList, vector<wstring>* MapCOLList, vector<string>* PropList);
	virtual void Free() override;
};

END