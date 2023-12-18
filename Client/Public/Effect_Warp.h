#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "BlendObject.h"

BEGIN(Client)

class CEffect_Warp final : public CBlendObject
{
private:
	CEffect_Warp(_dev pDevice, _context pContext);
	CEffect_Warp(const CEffect_Warp& rhs);
	virtual ~CEffect_Warp() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CVIBuffer_Instancing_Point* m_pVIBufferCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Warp* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END