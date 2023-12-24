#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CSlotBase_Tool final : public COrthographicObject
{
private:
	CSlotBase_Tool(_dev pDevice, _context pContext);
	CSlotBase_Tool(const CSlotBase_Tool& rhs);
	virtual ~CSlotBase_Tool() = default;

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
	CTexture* m_pBaseTextureCom{ nullptr };
	CTexture* m_pFrameTextureCom{ nullptr };
	CTexture* m_pSkillTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSlotBase_Tool* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

END