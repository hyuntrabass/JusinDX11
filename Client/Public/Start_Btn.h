#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CStart_Btn final : public COrthographicObject
{
	CStart_Btn(_dev pDevice, _context pContext);
	CStart_Btn(const CStart_Btn& rhs);
	virtual ~CStart_Btn() = default;

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
	_uint m_iIndex{};
	_vector m_Color{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CStart_Btn* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END