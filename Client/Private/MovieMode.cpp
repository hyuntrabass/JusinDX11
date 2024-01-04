#include "MovieMode.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"

CMovieMode::CMovieMode(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CMovieMode::CMovieMode(const CMovieMode& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CMovieMode::Init_Prototype()
{
	return S_OK;
}

HRESULT CMovieMode::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = static_cast<_float>(g_iWinSizeX);
	m_fSizeY = 40.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = 20.f;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CMovieMode::Tick(_float fTimeDelta)
{
	if (CTrigger_Manager::Get_Instance()->Hasto_PlayScene())
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

void CMovieMode::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CMovieMode::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	m_fSizeX = static_cast<_float>(g_iWinSizeX);
	m_fSizeY = static_cast<_float>(g_iWinSizeY);

	m_fX = static_cast<_float>(g_iWinSizeX >> 1);
	m_fY = static_cast<_float>(g_iWinSizeY >> 1);

	m_fDepth = 0.1f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Color_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	_float fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof fAlpha)))
	{
		return E_FAIL;
	}

	m_fSizeX = static_cast<_float>(g_iWinSizeX);
	m_fSizeY = 130.f;

	m_fX = static_cast<_float>(g_iWinSizeX >> 1);
	m_fY = m_fSizeY * 0.5f;

	m_fDepth = 0.1f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Color_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_fY = static_cast<_float>(g_iWinSizeY) - m_fY;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Color_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(TEXT("Font_Dialogue"), CUI_Manager::Get_Instance()->Get_SubTitle(), _float2(m_fX, m_fY), 0.6f);

	return S_OK;
}

HRESULT CMovieMode::Add_Components()
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

	return S_OK;
}

HRESULT CMovieMode::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	_float fAlpha = 1.f - m_fAppearance;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof fAlpha)))
	{
		return E_FAIL;
	}

	_float4 vColor{ 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMovieMode* CMovieMode::Create(_dev pDevice, _context pContext)
{
	CMovieMode* pInstance = new CMovieMode(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMovieMode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMovieMode::Clone(void* pArg)
{
	CMovieMode* pInstance = new CMovieMode(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMovieMode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMovieMode::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
