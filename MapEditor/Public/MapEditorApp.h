#pragma once
#include "MapEditor_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(MapEditor)

class CMapEditorApp final : public CBase
{
private:
	CMapEditorApp();
	virtual ~CMapEditorApp() = default;

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

public:
	static CMapEditorApp* Create();
	virtual void Free() override;
};

END