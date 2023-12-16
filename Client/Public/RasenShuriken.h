#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CRasenShuriken : public CBlendObject
{
private:
	enum RasenState
	{
		State_Charge,
		State_Shoot,
		State_Explode,
	};
private:
	CRasenShuriken(_dev pDevice, _context pContext);
	CRasenShuriken(const CRasenShuriken& rhs);
	virtual ~CRasenShuriken() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

public:
	void Shoot(_float3 vTargetPos);

private:
	static const _uint iNumModels{ 5 };
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom[iNumModels]{ nullptr };
	CTexture* m_pCoreMaskTextureCom{ nullptr };
	CTexture* m_pCircleMaskTextureCom{ nullptr };
	CTexture* m_pWingTrailMaskTextureCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

private:
	_float3* m_pPos{};
	_float2 m_vUVTransform{ 1.f, 0.f };

	RasenState m_eState{};
	_float m_fTimer{};
	_float3 m_vTargetPos{};
	_float m_fCoreScale{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRasenShuriken* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END