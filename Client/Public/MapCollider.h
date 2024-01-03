#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CMapCollider final : public CGameObject
{
private:
	CMapCollider(_dev pDevice, _context pContext);
	CMapCollider(const CMapCollider& rhs);
	virtual ~CMapCollider() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
#ifdef _DEBUGG
	virtual HRESULT Render() override;
#endif // _DEBUG


private:
	wstring m_strPrototypeTag{};
	
private:
	CModel* m_pModelCom{ nullptr };
#ifdef _DEBUGG
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

private:
	HRESULT Bind_ShaderResources();
#endif // _DEBUG
	HRESULT Add_Components();


public:
	static CMapCollider* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END