#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_CreateCharacter final : public CLevel
{
	CLevel_CreateCharacter(_dev pDevice, _context pContext);
	virtual ~CLevel_CreateCharacter() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Light();
	HRESULT Ready_Player();

public:
	static CLevel_CreateCharacter* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END