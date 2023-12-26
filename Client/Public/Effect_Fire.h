#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Fire final : public CGameObject
{
private:
	CEffect_Fire(_dev pDevice, _context pContext);
	CEffect_Fire(const CEffect_Fire& rhs);
	virtual ~CEffect_Fire() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CShader* m_pRectShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTexture* m_pDissolveTextureCom{ nullptr };
	CTexture* m_pBigMaskTextureCom{ nullptr };
	CTexture* m_pBigDiffTextureCom{ nullptr };
	CVIBuffer_Instancing_Point* m_pVIBufferCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferRectCom{ nullptr };

private:
	_float m_fLifeTime{};
	_int m_iSpriteIndex{};
	_float m_fDissolveRatio{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Fire* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END