#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Init(ifstream& ModelFile);

private:
	_char m_szName[MAX_PATH]{};

	_uint m_iNumkeyFrames{};
	vector<KEYFRAME> m_KeyFrames{};

public:
	static CChannel* Create(ifstream& ModelFile);
	virtual void Free() override;
};

END