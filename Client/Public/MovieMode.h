#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CMovieMode final : public COrthographicObject
{
private:
	CMovieMode(_dev pDevice, _context pContext);
	CMovieMode(const CMovieMode& rhs);
	virtual ~CMovieMode() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };

private:
	_float m_fAppearance{};
	_float m_fTimer{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMovieMode* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END