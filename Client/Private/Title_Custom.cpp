#include "Title_Custom.h"

CTitle_Custom::CTitle_Custom(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTitle_Custom::CTitle_Custom(const CTitle_Custom& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTitle_Custom::Init_Prototype()
{
	return S_OK;
}

HRESULT CTitle_Custom::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 1000.f * 0.7f;
	m_fSizeY = 100.f * 0.7f;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 1.f;

	m_fDepth = 1.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CTitle_Custom::Tick(_float fTimeDelta)
{
}

void CTitle_Custom::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
}

HRESULT CTitle_Custom::Render()
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

	return S_OK;
}

HRESULT CTitle_Custom::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_Title_Custom"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTitle_Custom::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CTitle_Custom* CTitle_Custom::Create(_dev pDevice, _context pContext)
{
	CTitle_Custom* pInstance = new CTitle_Custom(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTitle_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTitle_Custom::Clone(void* pArg)
{
	CTitle_Custom* pInstance = new CTitle_Custom(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTitle_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTitle_Custom::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
