#include "HpBar_Boss_Base.h"
#include "UI_Manager.h"

CHpBar_Boss_Base::CHpBar_Boss_Base(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHpBar_Boss_Base::CHpBar_Boss_Base(const CHpBar_Boss_Base& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHpBar_Boss_Base::Init_Prototype()
{
	return S_OK;
}

HRESULT CHpBar_Boss_Base::Init(void* pArg)
{
	if (pArg)
	{
		m_iType = *reinterpret_cast<_uint*>(pArg);
	}

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

void CHpBar_Boss_Base::Tick(_float fTimeDelta)
{
	wstring HPTag{};
	if (m_iType == 0)
	{
		HPTag = TEXT("Kurama");
	}
	else
	{
		HPTag = TEXT("Pain");
	}

	if (CUI_Manager::Get_Instance()->Get_HPRatio(HPTag) < 0)
	{
		m_fDissolve -= fTimeDelta;
		if (m_fDissolve < 0.f)
		{
			m_fDissolve = 0.f;
		}
	}
}

void CHpBar_Boss_Base::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHpBar_Boss_Base::Render()
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

	if (m_iType == 0)
	{
		m_fSizeX = 85.f * 1.45f;
		m_fSizeY = 85.f * 1.45f;

		m_fX = (g_iWinSizeX >> 1) - 179.f;
		m_fY = 75.f;
	}
	else
	{
		m_fSizeX = 196.f * 0.5f;
		m_fSizeY = 172.f * 0.5f;

		m_fX = (g_iWinSizeX >> 1) - 179.f;
		m_fY = 80.f;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pFaceTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
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

HRESULT CHpBar_Boss_Base::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Boss_Frame"), TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
	{
		return E_FAIL;
	}

	if (m_iType == 0)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Face_Kurama"), TEXT("Com_Texture_Face"), reinterpret_cast<CComponent**>(&m_pFaceTextureCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Face_Pain"), TEXT("Com_Texture_Face"), reinterpret_cast<CComponent**>(&m_pFaceTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CHpBar_Boss_Base::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	m_fSizeX = 580.f * 0.9f;
	m_fSizeY = 128.f * 0.9f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = 80.f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pFrameTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fDissolve, sizeof m_fDissolve)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHpBar_Boss_Base* CHpBar_Boss_Base::Create(_dev pDevice, _context pContext)
{
	CHpBar_Boss_Base* pInstance = new CHpBar_Boss_Base(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHpBar_Boss_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBar_Boss_Base::Clone(void* pArg)
{
	CHpBar_Boss_Base* pInstance = new CHpBar_Boss_Base(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHpBar_Boss_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Boss_Base::Free()
{
	__super::Free();

	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pFaceTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
