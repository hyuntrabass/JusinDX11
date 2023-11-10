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

	m_fSizeX = 512.f;
	m_fSizeY = 128.f;

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
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base_Frame"), TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
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
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
