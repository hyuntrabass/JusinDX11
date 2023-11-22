#pragma once
#include "Component.h"

BEGIN(Engine)

class CCollider final : public CComponent
{
private:
	CCollider(_dev pDevice, _context pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	void Update(_fmatrix WorldMatrix);
};

END