#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(_dev pDevice, _context pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	const _float& Get_CamDistance() const
	{
		return m_fCamDistance;
	}

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT Compute_CamDistance(class CTransform* pTransform);

protected:
	_float m_fCamDistance{};

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END