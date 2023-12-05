#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

class CChidori : public CBlendObject
{
private:
	CChidori(_dev pDevice, _context pContext);
	CChidori(const CChidori& rhs);
	virtual ~CChidori() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	_float3* m_pPos{};
	_float2 m_vUVTransform{ 1.f, 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CChidori* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

