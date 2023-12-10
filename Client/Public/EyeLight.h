#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CEyeLight final : public CBlendObject
{
private:
	CEyeLight(_dev pDevice, _context pContext);
	CEyeLight(const CEyeLight& rhs);
	virtual ~CEyeLight() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float3 vPos, _float fTimeDelta);
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

	CVIBuffer_Trail* m_pTrailBufferCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

private:
	list<_float3> m_TrailPosList{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEyeLight* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END