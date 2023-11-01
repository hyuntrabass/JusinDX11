#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Init(ifstream& ModelFile);

private:
	_char m_szName[MAX_PATH]{};
	_float m_fDuration{};
	_float m_fTickPerSec{};

	_uint m_iNumChannels{};
	vector<class CChannel*> m_Channels{};

public:
	static CAnimation* Create(ifstream& ModelFile);
	virtual void Free() override;
};

END