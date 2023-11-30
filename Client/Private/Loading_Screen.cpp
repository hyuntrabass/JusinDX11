#include "Loading_Screen.h"

CLoading_Screen::CLoading_Screen(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLoading_Screen::CLoading_Screen(const CLoading_Screen& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLoading_Screen::Init_Prototype()
{
	return S_OK;
}

HRESULT CLoading_Screen::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = 1.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_iTextureIndex = rand() % 7;

	return S_OK;
}

void CLoading_Screen::Tick(_float fTimeDelta)
{
}

void CLoading_Screen::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Priority, this);
}

HRESULT CLoading_Screen::Render()
{
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

	return S_OK;
}

HRESULT CLoading_Screen::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Screen"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Screen::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLoading_Screen* CLoading_Screen::Create(_dev pDevice, _context pContext)
{
	CLoading_Screen* pInstance = new CLoading_Screen(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLoading_Screen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_Screen::Clone(void* pArg)
{
	CLoading_Screen* pInstance = new CLoading_Screen(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoading_Screen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Screen::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
