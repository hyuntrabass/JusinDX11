#pragma once
#include "ModelConverter_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(ModelConverter)

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual ~CImguiMgr() = default;

public:
	HRESULT Init();
	void Tick();
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance { nullptr };

private:
	HRESULT Ready_Layers();

	HRESULT Load_Data();
	HRESULT Export_Data();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext, CGameInstance* pGameInstance);
	virtual void Free() override;
};

END