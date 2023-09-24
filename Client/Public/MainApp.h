#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev* m_pDevice{ nullptr };
	_context* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance{ nullptr };
	_float m_fTimeAcc{};
	_uint m_iFrameCount{};

private:
	HRESULT Open_Level(Level_ID eLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END