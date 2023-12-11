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
	_uint iTriggerNum{};
	class CDummy** ppDummy{ nullptr };
};

class CImguiMgr final : public CBase
{
	DECLARE_SINGLETON(CImguiMgr)
private:
	CImguiMgr();
	virtual ~CImguiMgr() = default;

public:
	const _float4& Get_Position() const;
	const _bool& HasTo_PlayeScene() const;
	CUTSCENE* Get_CurrentScene();

	void SetPos(const _float4& vPos, class CDummy* pDummy);
	void Select(const _float4& vPos, class CDummy* pDummy);
	void End_Scene();

	_bool ComputePickPos();
	_bool ComputeSelection();

public:
	HRESULT Init(_dev pDevice, _context pContext, vector<wstring>* MapList, vector<wstring>* MapCOLList, vector<string>* PropList);
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };

private: // for Input
	_bool m_isTextInputMode{};
	_int m_Curr_Stage{};
	ItemType m_eItemType{ ItemType::End };
	_float4 m_vPos{ 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLook{ 0.f, 0.f, 1.f, 0.f };
	const _char** m_pItemList_Props{ nullptr };
	_int m_iNumProps{};
	const _char* const m_pItemList_Monster[2]
	{
		"Sandman",
		"SandNinja",
	};
	const _char* const m_pItemList_NPC[3]
	{
		"Kakashi",
		"Naruto",
		"Sakura",
	};

private:
	class C3D_Cursor* m_pCursor{ nullptr };
	list<class CDummy*> m_PropList{};
	list<class CDummy*> m_MonsterList{};
	_bool m_ComputePickPos{};
	_bool m_ComputeSelection{};
	_float m_fCamDist{};
	class CDummy* m_pSelectedDummy{ nullptr };
	vector<wstring>* m_pMapModels{ nullptr };
	vector<wstring>* m_pMapCOLModels{ nullptr };

	list<_float4> m_TriggerList{};
	vector<const _char*> m_TriggerItemList{};

	_bool m_isRecording{};
	_float m_fRecTimer{};
	vector<CUTSCENE*> m_Scenes{};
	vector<const _char*> m_SceneList{};
	_bool m_PlayScene{};
	CUTSCENE* m_pCurrentScene{ nullptr };

private:
	HRESULT Ready_Layers(vector<string>* pPropCount);
	void Create_Dummy(const _int& iListIndex);
	void Delete_Dummy();

	HRESULT Load_Data();
	HRESULT Export_Data();
	HRESULT Export_Trigger_Data();
	HRESULT Export_Monster_Data();

public:
	virtual void Free() override;
};

END