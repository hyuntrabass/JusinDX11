#include "HpBar_Player_Base.h"

CHpBar_Player_Base::CHpBar_Player_Base(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHpBar_Player_Base::CHpBar_Player_Base(const CHpBar_Player_Base& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHpBar_Player_Base::Init_Prototype()
{
	return S_OK;
}

HRESULT CHpBar_Player_Base::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 512.f * 0.8f;
	m_fSizeY = 128.f * 0.8f;

	m_fX = 250.f;
	m_fY = 620.f;

	m_fDepth = 1.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CHpBar_Player_Base::Tick(_float fTimeDelta)
{
}

void CHpBar_Player_Base::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHpBar_Player_Base::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_fSizeX = 85.f * 0.8f;
	m_fSizeY = 85.f * 0.8f;

	m_fX = 125.f;
	m_fY = 625.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSkillTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Texture)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_fSizeX = 128.f * 0.8f;
	m_fSizeY = 128.f * 0.8f;

	m_fX = 125.f;
	m_fY = 625.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGaugeBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Texture)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGaugeMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	_float4 vColor{ 0.f, 1.f, 1.f, 1.f, };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHpBar_Player_Base::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pBaseTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base_Frame"), TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_Mask"), TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_Gauge_Base"), TEXT("Com_Texture_Gauge_Base"), reinterpret_cast<CComponent**>(&m_pGaugeBaseTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_Gauge_Mask"), TEXT("Com_Texture_Gauge_Mask"), reinterpret_cast<CComponent**>(&m_pGaugeMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_RasenShuri"), TEXT("Com_Texture_Skill"), reinterpret_cast<CComponent**>(&m_pSkillTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHpBar_Player_Base::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	m_fSizeX = 512.f * 0.8f;
	m_fSizeY = 128.f * 0.8f;

	m_fX = 250.f;
	m_fY = 620.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pFrameTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHpBar_Player_Base* CHpBar_Player_Base::Create(_dev pDevice, _context pContext)
{
	CHpBar_Player_Base* pInstance = new CHpBar_Player_Base(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHpBar_Player_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBar_Player_Base::Clone(void* pArg)
{
	CHpBar_Player_Base* pInstance = new CHpBar_Player_Base(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHpBar_Player_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Player_Base::Free()
{
	__super::Free();

	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pBaseTextureCom);
	Safe_Release(m_pSkillTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pGaugeBaseTextureCom);
	Safe_Release(m_pGaugeMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
