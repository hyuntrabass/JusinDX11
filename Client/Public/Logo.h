#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CLogo final : public COrthographicObject
{
	CLogo(_dev pDevice, _context pContext);
	CLogo(const CLogo& rhs);
	virtual ~CLogo() = default;

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

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLogo* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END