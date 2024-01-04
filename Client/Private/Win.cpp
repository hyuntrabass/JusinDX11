#include "Win.h"

CWin::CWin(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CWin::CWin(const CWin& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CWin::Init_Prototype()
{
	return S_OK;
}

HRESULT CWin::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 2516.f * 0.2f;
	m_fSizeY = 1813.f * 0.2f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_pGameInstance->StopSound(SCH_EFFECT);
	m_pGameInstance->Play_Sound(TEXT("Shinobi_Execution"), SCH_EFFECT, 0.5f);

	return S_OK;
}

void CWin::Tick(_float fTimeDelta)
{
	if (m_fTimer < 4.f)
	{
		m_fAppearance += fTimeDelta;
	}
	else
	{
		m_fAppearance -= fTimeDelta;
	}

	m_fTimer += fTimeDelta;
	
	if (m_fAppearance > 1.f)
	{
		m_fAppearance = 1.f;
	}
	else if (m_fAppearance < 0.f)
	{
		m_fAppearance = 0.f;
		m_isDead = true;
	}
}

void CWin::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CWin::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWin::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Win"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWin::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAppearance, sizeof m_fAppearance)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CWin* CWin::Create(_dev pDevice, _context pContext)
{
	CWin* pInstance = new CWin(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWin::Clone(void* pArg)
{
	CWin* pInstance = new CWin(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWin::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
