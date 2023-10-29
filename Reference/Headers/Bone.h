#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

private:
	HRESULT Init(ifstream& ModelFile);

private:
	_float4x4 m_TransformationMatrix{};
	_float4x4 m_CombindTransformationMatrix{};

	_uint m_iParentIndex{};

public:
	static CBone* Create(ifstream& ModelFile);
	virtual void Free() override;
};

END