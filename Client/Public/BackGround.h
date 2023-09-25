#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
private:
	CBackGround(_dev pDevice, _context pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	_float m_fSizeX{}, m_fSizeY{};
	_float m_fX{}, m_fY{};
	_float4x4 m_ViewMatrix{}, m_ProjMatrix{};

private:
	HRESULT Add_Components();

public:
	static CBackGround* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END