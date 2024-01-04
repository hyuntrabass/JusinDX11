#include "Indicator.h"

CIndicator::CIndicator(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CIndicator::CIndicator(const CIndicator& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CIndicator::Init_Prototype()
{
	return S_OK;
}

HRESULT CIndicator::Init(void* pArg)
{
	m_isBoss = *reinterpret_cast<_bool*>(pArg);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 64.f;
	m_fSizeY = 64.f;

	if (not m_isBoss)
	{
		m_fSizeX *= 0.5f;
		m_fSizeY *= 0.5f;
	}

	m_fDepth = 0.3f;
	m_iHP = 1;

	return S_OK;
}

void CIndicator::Tick(_float2 vPos)
{
	vPos.x = clamp(vPos.x, 0.f + m_fSizeX * 0.5f, static_cast<_float>(g_iWinSizeX) - m_fSizeX * 0.5f);
	vPos.y = clamp(vPos.y, 0.f + m_fSizeY * 0.5f, static_cast<_float>(g_iWinSizeY) - m_fSizeY * 0.5f);

	m_fX = vPos.x;
	m_fY = vPos.y;

	if (vPos.x < 0.f)
	{
		m_isOut = true;
	}
	else
	{
		m_isOut = false;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CIndicator::Late_Tick(_float fTimeDelta)
{
	if (m_isOut)
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CIndicator::Render()
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

	return S_OK;
}

HRESULT CIndicator::Add_Components()
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

	if (m_isBoss)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Indicator_Boss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Indicator"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CIndicator::Bind_ShaderResources()
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

CIndicator* CIndicator::Create(_dev pDevice, _context pContext)
{
	CIndicator* pInstance = new CIndicator(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CIndicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIndicator::Clone(void* pArg)
{
	CIndicator* pInstance = new CIndicator(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CIndicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIndicator::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
