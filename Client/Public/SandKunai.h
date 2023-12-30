#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSandKunai : public CGameObject
{
private:
	CSandKunai(_dev pDevice, _context pContext);
	CSandKunai(const CSandKunai& rhs);
	virtual ~CSandKunai() = default;

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
	CCollider* m_pColliderCom{ nullptr };
	class CCommonTrail* m_pTrail{ nullptr };

private:
	_float m_fTimer{};

private:
	HRESULT Add_Components();
	HRESULT Bind_Shader_Resources();

public:
	static CSandKunai* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END