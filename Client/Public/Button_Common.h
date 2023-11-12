#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CButton_Common final : public COrthographicObject
{
	CButton_Common(_dev pDevice, _context pContext);
	CButton_Common(const CButton_Common& rhs);
	virtual ~CButton_Common() = default;

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
	class CUI_Manager* m_pUI_Manager{ nullptr };

private:
	_uint m_iButtonType{};
	wstring m_strButtonTag{};
	_uint m_iIndex{};
	_vector m_Color{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CButton_Common* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END