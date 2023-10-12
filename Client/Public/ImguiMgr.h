#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(MapEditor)

class CImguiMgr final : public CBase
{
private:
	CImguiMgr(_dev pDevice, _context pContext);
	virtual ~CImguiMgr() = default;

public:
	HRESULT Init();
	HRESULT DrawEditor();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	void Tick();

public:
	static CImguiMgr* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END