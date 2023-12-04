#pragma once
#include "Component.h"

BEGIN(Engine)

enum RenderGroup
{
	RG_Priority,
	RG_NonLight,
	RG_NonBlend,
	RG_Blend,
	RG_UI,
	RG_Cursor,
	RG_End
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
	list<class CGameObject*> m_RenderObject[RG_End]{};

private:
	class CVIBuffer_Rect* m_pVIBuffer{ nullptr };
	class CShader* m_pShader{ nullptr };

	_float44 m_ViewMatrix{}, m_ProjMatrix{};

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static CRenderer* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END