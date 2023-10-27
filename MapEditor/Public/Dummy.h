#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "ImguiMgr.h"

BEGIN(MapEditor)

class CDummy final : public CGameObject
{
private:
	CDummy(_dev pDevice, _context pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	void Select(const _bool& isSelected);

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModel{ nullptr };
	//CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	//CTexture* m_pTextureCom{ nullptr };

private:
	DummyInfo m_Info{};
	_bool m_isSelected{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END