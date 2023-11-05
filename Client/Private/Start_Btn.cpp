#include "Start_Btn.h"

CStart_Btn::CStart_Btn(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CStart_Btn::CStart_Btn(const CStart_Btn& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CStart_Btn::Init_Prototype()
{
	return S_OK;
}

HRESULT CStart_Btn::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 740 * 0.75f;
	m_fSizeY = 90 * 0.75f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY, 0.9f);

	return S_OK;
}

void CStart_Btn::Tick(_float fTimeDelta)
{
	GET_CURSOR_POINT(pt);
	RECT rc = { static_cast<_long>(m_fX - m_fSizeX * 0.5f), 317, static_cast<_long>(m_fX + m_fSizeX * 0.5f), static_cast<_long>(m_fY) };
	if (PtInRect(&rc, pt))
	{
		m_iIndex = 1;
		m_Color = Colors::Gold;
	}
	else
	{
		m_Color = Colors::White;
		m_iIndex = 0;
	}
}

void CStart_Btn::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
}

HRESULT CStart_Btn::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(1)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	m_pGameInstance->Render_Text(TEXT("Font_Malang"), TEXT("게임 시작"), _float2(m_fX, m_fY), 0.7f, m_Color);
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Begin(0)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pVIBufferCom->Render()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CStart_Btn::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_StartBtn"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStart_Btn::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceArray(m_pShaderCom, "g_TextureArray")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TexIndex", &m_iIndex, sizeof m_iIndex)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CStart_Btn* CStart_Btn::Create(_dev pDevice, _context pContext)
{
	CStart_Btn* pInstance = new CStart_Btn(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CStart_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStart_Btn::Clone(void* pArg)
{
	CStart_Btn* pInstance = new CStart_Btn(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CStart_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStart_Btn::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
