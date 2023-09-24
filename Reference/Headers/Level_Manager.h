#pragma once
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Init();
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	class CLevel* m_pCurrentLevel{ nullptr };
	_uint m_iCurrentLevelIndex{};

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END