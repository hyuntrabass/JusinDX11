#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL COrthographicObject abstract : public CGameObject
{
protected:
	COrthographicObject(_dev pDevice, _context pContext);
	COrthographicObject(const COrthographicObject& rhs);
	virtual ~COrthographicObject() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	void Apply_Orthographic(_uint iWinSizeX, _uint iWinSizeY, _float fPosZ = 0.f);

protected:
	_float m_fSizeX{}, m_fSizeY{};
	_float m_fX{}, m_fY{};
	_float4x4 m_ViewMatrix{}, m_ProjMatrix{};

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END