#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CHpBar_Player final : public COrthographicObject
{
private:
	CHpBar_Player(_dev pDevice, _context pContext);
	CHpBar_Player(const CHpBar_Player& rhs);
	virtual ~CHpBar_Player() = default;

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
	CTexture* m_pFrameTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom[2]{ nullptr };

private:
	_float m_fHPRatio{};
	_bool m_hasRatioChanged{};
	_float4 m_vColor{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHpBar_Player* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END