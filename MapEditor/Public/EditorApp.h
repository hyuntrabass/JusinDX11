#pragma once
#include "MapEditor_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(MapEditor)

class CEditorApp final : public CBase
{
private:
	CEditorApp();
	virtual ~CEditorApp() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	class CImguiMgr* m_pImguiMgr{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	CRenderer* m_pRenderer{ nullptr };
	_float m_fTimeAcc{};
	_uint m_iFrameCount{};
	_uint m_iPropCount{};
	vector<string> m_PropList{};
	vector<wstring> m_MapList[3]{};
	vector<wstring> m_MapCOLList[3]{};

private:
	HANDLE m_hThread{};
	CRITICAL_SECTION m_Critical_Section{};
	_bool m_bLoadComplete{};
	wstring m_strLoadingText{};
	_uint m_iNumDots{};
	_float m_fLoadingTime{};

private:
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Loading_Screen();

public:
	void Begin_Thread();
	HRESULT Ready_Prototype_GameObject();
	void End_Thread();

public:
	static CEditorApp* Create();
	virtual void Free() override;
};

END