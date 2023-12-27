#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CEffect_Smoke final : public CBlendObject
{
private:
	CEffect_Smoke(_dev pDevice, _context pContext);
	CEffect_Smoke(const CEffect_Smoke& rhs);
	virtual ~CEffect_Smoke() = default;

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
	_float m_fLifeTime{};
	_int m_iSpriteIndex{};
	_float m_fDissolveRatio{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Smoke* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END