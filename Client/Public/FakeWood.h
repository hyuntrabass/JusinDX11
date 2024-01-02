#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CFakeWood final : public CGameObject
{
private:
	CFakeWood(_dev pDevice, _context  pContext);
	CFakeWood(const CFakeWood& rhs);
	virtual ~CFakeWood() = default;

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

private:
	_float4 m_vPos{};
	_float m_fGravity{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFakeWood* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END