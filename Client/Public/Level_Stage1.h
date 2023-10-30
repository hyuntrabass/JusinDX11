#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage1 final : public CLevel
{
private:
	CLevel_Stage1(_dev pDevice, _context pContext);
	virtual ~CLevel_Stage1() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Map();

public:
	static CLevel_Stage1* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END