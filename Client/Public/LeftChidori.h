#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CLeftChidori : public CBlendObject
{
private:
	CLeftChidori(_dev pDevice, _context pContext);
	CLeftChidori(const CLeftChidori& rhs);
	virtual ~CLeftChidori() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	static const _uint m_iNumModels{ 4 };
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom[m_iNumModels]{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	_float3 m_vPos{};
	_float m_fUVTransform{};
	_float m_fTimer{};
	_uint m_iModelNumber{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLeftChidori* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END