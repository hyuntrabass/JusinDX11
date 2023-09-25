#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(_dev pDevice, _context pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Init(Level_ID eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	Level_ID m_eNextLevel{ Level_ID::End };
	class CLoader* m_pLoader{ nullptr };

public:
	static CLevel_Loading* Create(_dev pDevice, _context pContext, Level_ID eNextLevel);
	virtual void Free() override;
};

END