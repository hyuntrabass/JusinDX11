#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CFootEffect final : public CBlendObject
{
private:
	CFootEffect(_dev pDevice, _context pContext);
	CFootEffect(const CFootEffect& rhs);
	virtual ~CFootEffect() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float3 vPos, _float fTimeDelta);
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	class CCommonTrail* m_pTrail{ nullptr };
	_float m_fTrailTimer{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFootEffect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END