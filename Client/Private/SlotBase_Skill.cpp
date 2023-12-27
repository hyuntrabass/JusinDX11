#include "SlotBase_Skill.h"
#include "UI_Manager.h"

CSlotBase_Skill::CSlotBase_Skill(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSlotBase_Skill::CSlotBase_Skill(const CSlotBase_Skill& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSlotBase_Skill::Init_Prototype()
{
	return S_OK;
}

HRESULT CSlotBase_Skill::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
	}

	m_pUIManager = CUI_Manager::Get_Instance();
	Safe_AddRef(m_pUIManager);

	m_iSkillNumber = *reinterpret_cast<_uint*>(pArg);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 94.f * 0.8f;
	m_fSizeY = 94.f * 0.8f;

	m_fX = 1030.f + m_fSizeX * m_iSkillNumber;
	m_fY = 630.f;

	m_fDepth = 1.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CSlotBase_Skill::Tick(_float fTimeDelta)
{
	if (not m_pUIManager->Is_SkillUsable(m_iSkillNumber))
	{
		if (m_fReadyRatio < -0.5f)
		{
			m_fReadyRatio = 0.5f;
		}
		m_fReadyRatio -= fTimeDelta / 7.f;
	}
	
	if (m_fReadyRatio < - 0.5f)
	{
		m_pUIManager->Skill_Ready(m_iSkillNumber);
	}
}

void CSlotBase_Skill::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSlotBase_Skill::Render()
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

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSkillTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGaugeMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_SkillReadyTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &m_fReadyRatio, sizeof m_fReadyRatio)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Inv_Mask_Texture)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSlotBase_Skill::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Skill"), TEXT("Com_BaseTexture"), reinterpret_cast<CComponent**>(&m_pBaseTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Skill_Frame"), TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Mask"), TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_GaugeMask"), TEXT("Com_Texture_GaugeMask"), reinterpret_cast<CComponent**>(&m_pGaugeMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (m_iSkillNumber == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Chidori"), TEXT("Com_Texture_Skill"), reinterpret_cast<CComponent**>(&m_pSkillTextureCom))))
		{
			return E_FAIL;
		}
	}
	else if (m_iSkillNumber == 2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Fireball"), TEXT("Com_Texture_Skill"), reinterpret_cast<CComponent**>(&m_pSkillTextureCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_RasenShuri"), TEXT("Com_Texture_Skill"), reinterpret_cast<CComponent**>(&m_pSkillTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSlotBase_Skill::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

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

CSlotBase_Skill* CSlotBase_Skill::Create(_dev pDevice, _context pContext)
{
	CSlotBase_Skill* pInstance = new CSlotBase_Skill(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlotBase_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlotBase_Skill::Clone(void* pArg)
{
	CSlotBase_Skill* pInstance = new CSlotBase_Skill(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlotBase_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlotBase_Skill::Free()
{
	__super::Free();

	Safe_Release(m_pUIManager);
	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pBaseTextureCom);
	Safe_Release(m_pSkillTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
