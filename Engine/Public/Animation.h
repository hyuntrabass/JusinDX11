#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Init(ifstream& ModelFile);
	_bool Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged);

private:
	_char m_szName[MAX_PATH]{};
	
	_float m_fDuration{};
	_float m_fTickPerSec{};
	_float m_fCurrentAnimPos{};
	//vector<_uint> m_CurrentKeyFrames{};
	//vector<_float4x4> m_PrevTransformations{};

	_uint m_iNumChannels{};
	vector<class CChannel*> m_Channels{};

	_bool m_isFinished{};
	_bool m_isInterpolating{};

public:
	static CAnimation* Create(ifstream& ModelFile);
	CAnimation* Clone();
	virtual void Free() override;
};

END