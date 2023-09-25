#pragma once
#include "Component.h"

BEGIN(Engine)

enum class RenderGroup
{
	Priority,
	NonLight,
	NonBlend,
	Blend,
	UI,
	Cursor,
	End
};

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	CRenderer(_dev pDevice, _context pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RenderGroup eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw_RenderGroup();

private:
	list<class CGameObject*> m_RenderObject[ToIndex(RenderGroup::End)];

public:
	static CRenderer* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END