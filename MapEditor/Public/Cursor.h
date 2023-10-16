#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CCursor final : public CGameObject
{
private:
	CCursor(_dev pDevice, _context pContext);
	CCursor(const CCursor& rhs);
	virtual ~CCursor() = default;

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
	_float* m_pPos{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCursor* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END