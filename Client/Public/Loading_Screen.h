#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CLoading_Screen final : public COrthographicObject
{
private:
	CLoading_Screen(_dev pDevice, _context pContext);
	CLoading_Screen(const CLoading_Screen& rhs);
	virtual ~CLoading_Screen() = default;

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
	_uint m_iTextureIndex{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLoading_Screen* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END