#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CAim final : public COrthographicObject
{
private:
	CAim(_dev pDevice, _context pContext);
	CAim(const CAim& rhs);
	virtual ~CAim() = default;

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
	_float m_fAngle{};
	_float m_fScale{};
	_float m_fAppearance{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAim* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END