#pragma once
#include "Client_Define.h"
#include "BodyPart.h"

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
	void Set_ButtonState(const wstring& strButtonTag, const _bool& bState);
	void Set_HP(const wstring& strHPTag, _uint iMaxHP, _uint iCurrHP);
	void Create_Aim();
	void Delete_Aim();

	const _bool Is_ButtonPushed(_uint iIndex) const;
	const _bool is_Activated(_uint iIndex) const;
	const _uint& Get_PartIndex(PART_TYPE eType) const;
	const _uint& Get_PageIndex() const;
	const _uint Get_sizeofButtons() const;
	const _float& Get_HPRatio(const wstring& strHPTag);

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Ready_UI_Logo();
	HRESULT Ready_UI_Custom();

private:
	CGameInstance* m_pGameInstance{ nullptr };

	class CAim* m_pAim{ nullptr };
	map<const wstring, _float> m_HPs{};
	vector<class CButton_Common*> m_Buttons{};
	_uint m_iButtonIndex{};
	_uint m_iPageIndex{};
	_uint m_iScroll{};
	_uint m_iNumButtons{};

	_uint m_iPartIndex[PT_END]{};

private:
	void Customization();
	void Clear_Buttons();

public:
	virtual void Free() override;
};

END