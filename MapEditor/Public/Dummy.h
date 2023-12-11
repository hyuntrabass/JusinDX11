#pragma once
#include "MapEditor_Define.h"
#include "BlendObject.h"
#include "ImguiMgr.h"

BEGIN(MapEditor)

class CDummy final : public CBlendObject
{
private:
	CDummy(_dev pDevice, _context pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	void Select(const _bool& isSelected);
	void Modify(_fvector vPos, _fvector vLook);
	
	void Get_State(_float4& vPos, _float4& vLook);
	
	HRESULT WriteFile(ofstream& File, _bool WriteWData = false, _uint iWData = 0);
	HRESULT WriteMonsterFile(ofstream& File, _bool WriteWData = false, _uint iWData = 0);

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
	CImguiMgr* m_pImguiMgr{ nullptr };

	DummyInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_bool m_isAnim{};
	ANIM_DESC m_Animation{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END