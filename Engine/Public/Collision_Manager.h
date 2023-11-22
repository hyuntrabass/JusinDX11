#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager() = default;
	virtual ~CCollision_Manager() = default;

public:
	_bool IsCollided(class CTransform* pTransform);

private:

public:
	virtual void Free() override;
};

END