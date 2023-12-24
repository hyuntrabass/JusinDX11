#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CHpBar_Player_Base final : public COrthographicObject
{
private:
	CHpBar_Player_Base(_dev pDevice, _context pContext);
	CHpBar_Player_Base(const CHpBar_Player_Base& rhs);
	virtual ~CHpBar_Player_Base() = default;

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
	CTexture* m_pBaseTextureCom{ nullptr };
	CTexture* m_pFrameTextureCom{ nullptr };
	CTexture* m_pSkillTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTexture* m_pGaugeBaseTextureCom{ nullptr };
	CTexture* m_pGaugeMaskTextureCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHpBar_Player_Base* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END