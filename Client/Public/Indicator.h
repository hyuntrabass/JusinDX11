#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CIndicator final : public COrthographicObject
{
private:
	CIndicator(_dev pDevice, _context pContext);
	CIndicator(const CIndicator& rhs);
	virtual ~CIndicator() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float2 vPos);
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_bool m_isBoss{};
	_float m_fAngle{};
	_float m_fScale{};
	_float m_fAppearance{};
	_bool m_isOut{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CIndicator* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END