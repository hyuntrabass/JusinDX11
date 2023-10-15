#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"

BEGIN(MapEditor)

struct DummyInfo
{
	_float4 vPos{};
	_float4 vLook{};
};

class CDummy final : public CGameObject
{
	CDummy(_dev pDevice, _context pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END