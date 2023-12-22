#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CHit final : public COrthographicObject
{
private:
	CHit(_dev pDevice, _context pContext);
	CHit(const CHit& rhs);
	virtual ~CHit() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint Add_Hit();

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pNumTextureCom{ nullptr };
	CTexture* m_pHitTextureCom{ nullptr };
	CTexture* m_pSmallHitsTextureCom{ nullptr };
	CTexture* m_pFxTextureCom[2]{ nullptr };

private:
	_float2 m_vNumSize{};
	_float2 m_vSmallHitSize{};
	_float2 m_vHitSize{};
	_float2 m_vFxSize[2]{};
	_float m_fSizeRatio{};
	_float44 m_Rotation{};

	_uint m_iCombo{ 1 };
	_float m_fTimer{};
	_float m_fAlpha{};
	_float m_fFxAlpha{};
	_float m_fFxRotation{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHit* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END