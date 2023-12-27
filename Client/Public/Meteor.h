#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CMeteor : public CBlendObject
{
private:
	enum MeteorState
	{
		State_Charge,
		State_Shoot,
		State_Summon,
		State_Dissolve,
	};
private:
	CMeteor(_dev pDevice, _context pContext);
	CMeteor(const CMeteor& rhs);
	virtual ~CMeteor() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

public:
	void Shoot();

private:
	static const _uint m_iNumModels{ 5 };
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom[m_iNumModels]{ nullptr };
	CTexture* m_pCoreMaskTextureCom{ nullptr };
	CTexture* m_pEffectMaskTextureCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

private:
	_float3* m_pPos{};
	_float3 m_vUVTransform{ 1.f, 0.f, -1.f };

	MeteorState m_eState{};
	_float m_fTimer{};
	_float3 m_vTargetPos{};
	_bool m_hasTarget{};
	_float m_fCoreScale{};
	_float3 m_vAngle[8]{};
	_bool m_bRotate[4]{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMeteor* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END