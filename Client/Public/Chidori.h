#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CChidori : public CBlendObject
{
private:
	CChidori(_dev pDevice, _context pContext);
	CChidori(const CChidori& rhs);
	virtual ~CChidori() = default;

public:
	void Set_RushingState(const _bool isRushing);

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	static const _uint m_iNumModels{ 9 };
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom[m_iNumModels]{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

private:
	_float44* m_pSocketMatrix{};
	_float44 m_WorldMatrix{};
	_float3 m_vUVTransform{ 1.f, 0.f , -1.f };
	_float m_fTimer{};
	_float m_fTraceTimer{};
	LIGHT_DESC m_OriginMainLight{};
	LIGHT_DESC m_OriginPlayerLight{};
	_bool m_isRushing{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CChidori* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END