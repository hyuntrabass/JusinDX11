#include "HpBar_Boss.h"
#include "UI_Manager.h"

CHpBar_Boss::CHpBar_Boss(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHpBar_Boss::CHpBar_Boss(const CHpBar_Boss& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHpBar_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CHpBar_Boss::Init(void* pArg)
{
	if (pArg)
	{
		m_iType = *reinterpret_cast<_uint*>(pArg);
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 512.f * 1.11f;
	m_fSizeY = 64.f * 1.f;

	m_fX = (g_iWinSizeX >> 1) + 32.f;
	m_fY = 82.f;

	m_fDepth = 0.8f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CHpBar_Boss::Tick(_float fTimeDelta)
{
}

void CHpBar_Boss::Late_Tick(_float fTimeDelta)
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
	_float fHPRatio{ CUI_Manager::Get_Instance()->Get_HPRatio(HPTag) };

	if (fHPRatio < 0.f)
	{
		fHPRatio = 0.f;
		m_fDissolve -= fTimeDelta;
		return;
	}

	_vector vSrcColor{ XMVectorSet(1.f, 0.f, 0.f, 1.f) }, vMiddleColor{ XMVectorSet(1.f, 1.f, 0.f, 1.f) }, vDstColor{ XMVectorSet(0.f, 1.f, 0.4f, 1.f) };

	if (fHPRatio > 0.5f)
	{
		XMStoreFloat4(&m_vColor, XMVectorLerp(vMiddleColor, vDstColor, (fHPRatio - 0.5f) * 2.f));
	}
	else
	{
		XMStoreFloat4(&m_vColor, XMVectorLerp(vSrcColor, vMiddleColor, fHPRatio * 2.f));
	}

	fHPRatio = fHPRatio * (0.82f - 0.15f) + 0.15f;

	if (fabs(m_fHPRatio - fHPRatio) > 0.01f)
	{
		if (m_fHPRatio < fHPRatio)
		{
			m_fHPRatio += 0.5f * fTimeDelta;
		}
		else
		{
			m_fHPRatio -= 0.5f * fTimeDelta;
		}
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHpBar_Boss::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &m_fHPRatio, sizeof m_fHPRatio)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_HP)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHpBar_Boss::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Frame"), TEXT("Com_FrameTexture"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Mask"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Mask2"), TEXT("Com_MaskTexture2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom[1]))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHpBar_Boss::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fDissolve, sizeof m_fDissolve)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHpBar_Boss* CHpBar_Boss::Create(_dev pDevice, _context pContext)
{
	CHpBar_Boss* pInstance = new CHpBar_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHpBar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBar_Boss::Clone(void* pArg)
{
	CHpBar_Boss* pInstance = new CHpBar_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHpBar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pMaskTextureCom[0]);
	Safe_Release(m_pMaskTextureCom[1]);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
