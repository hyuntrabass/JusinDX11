#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CKunai final : public CGameObject
{
private:
	CKunai(_dev pDevice, _context pContext);
	CKunai(const CKunai& rhs);
	virtual ~CKunai() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _bool& HasStoppe();

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_Shader_Resources();

private:
	_float4 m_vTargetPos{};
	_float m_fLifeTimer{};
	_bool m_hasStopped{};
	_bool m_bFail{};

public:
	static CKunai* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END