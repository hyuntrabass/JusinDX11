#include "SlotBase_Skill.h"

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
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 94.f;
	m_fSizeY = 94.f;

	m_fX = 1180.f;
	m_fY = 630.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY, 1.f);

	return S_OK;
}

void CSlotBase_Skill::Tick(_float fTimeDelta)
{
}

void CSlotBase_Skill::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
}

HRESULT CSlotBase_Skill::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
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
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_SlotBase_Skill"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_SlotBase_Skill_Frame"), TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
	{
		return E_FAIL;
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

	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
