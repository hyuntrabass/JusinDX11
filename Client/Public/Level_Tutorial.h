#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Tutorial final : public CLevel
{
private:
	CLevel_Tutorial(_dev* pDevice, _context* pContext);
	virtual ~CLevel_Tutorial() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_Tutorial* Create(_dev* pDevice, _context* pContext);
	virtual void Free() override;
};

END