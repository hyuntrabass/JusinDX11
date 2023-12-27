#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CEffect_Impact final : public CBlendObject
{
private:
	CEffect_Impact(_dev pDevice, _context pContext);
	CEffect_Impact(const CEffect_Impact& rhs);
	virtual ~CEffect_Impact() = default;

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
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	_float m_fTimeAcc{};
	_float m_fMaxSize{};
	_float4 m_vColor{};	
	_float m_fAlpha{ 1.f };
	_uint m_iType{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Impact* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END