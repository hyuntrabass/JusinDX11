#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect final : public CGameObject
{
private:
	CEffect(_dev pDevice, _context pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };
	CVIBuffer_Instancing_Point* m_pVIBufferCom{ nullptr };

private:
	_float m_fLifeTime{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END