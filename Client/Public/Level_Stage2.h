#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage2 final : public CLevel
{
private:
	CLevel_Stage2(_dev pDevice, _context pContext);
	virtual ~CLevel_Stage2() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Map();

public:
	static CLevel_Stage2* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END