#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
private:
	CUI_Manager() = default;
	virtual ~CUI_Manager() = default;

public:
	void Register_Button(_uint iLevelIndex, const wstring& strButtonTag);

	void Set_ButtonState(_uint iLevelIndex, const wstring& strButtonTag, const _bool& bState);

	const _bool Get_ButtonState(_uint iLevelIndex, const wstring& strButtonTag) const;
	const _bool is_Activated() const;

public:
	HRESULT Init(CGameInstance* pGameInstance);

public:
	HRESULT Ready_UI_Logo();
	HRESULT Ready_UI_Tuto();

private:
	CGameInstance* m_pGameInstance{ nullptr };

	map<const wstring, _bool> m_pButtons[LEVEL_END]{};
	static _uint m_iButtonIDs[LEVEL_END];

public:
	virtual void Free() override;
};

END