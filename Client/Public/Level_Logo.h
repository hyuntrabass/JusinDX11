#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(_dev pDevice, _context pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Logo(const wstring& strLayerTag);
	HRESULT Ready_Layer_Buttons(const wstring& strLayerTag);

public:
	static CLevel_Logo* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END