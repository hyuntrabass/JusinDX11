#pragma once
#include "MapEditor_Define.h"
#include "Level.h"

BEGIN(MapEditor)

class CLevel_Editor final : public CLevel
{
private:
	CLevel_Editor(_dev pDevice, _context pContext);
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer();

public:
	static CLevel_Editor* Create(_dev pDevice, _context pContext);
	virtual void Free() override;

};

END