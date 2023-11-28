#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CMiniBomb final : public CGameObject
{
private:
	CMiniBomb(_dev pDevice, _context pContext);
	CMiniBomb(const CMiniBomb& rhs);
	virtual ~CMiniBomb() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CModel* m_pEffectModelCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_Shader_Resources();

private:
	_float m_fLifeTimer{};

public:
	static CMiniBomb* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END