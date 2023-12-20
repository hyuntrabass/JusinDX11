#include "HpBar_Player.h"
#include "UI_Manager.h"

CHpBar_Player::CHpBar_Player(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHpBar_Player::CHpBar_Player(const CHpBar_Player& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHpBar_Player::Init_Prototype()
{
	return S_OK;
}

HRESULT CHpBar_Player::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 512.f;
	m_fSizeY = 64.f;

	m_fX = 305.f;
	m_fY = 645.f;

	m_fDepth = 0.8f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CHpBar_Player::Tick(_float fTimeDelta)
{
}

void CHpBar_Player::Late_Tick(_float fTimeDelta)
{
	_float fHPRatio{ CUI_Manager::Get_Instance()->Get_HPRatio(TEXT("Player")) };

	_vector vSrcColor{ XMVectorSet(1.f, 0.f, 0.f, 1.f) }, vMiddleColor{ XMVectorSet(1.f, 1.f, 0.f, 1.f) }, vDstColor{ XMVectorSet(0.f, 1.f, 0.4f, 1.f) };

	if (fHPRatio > 0.5f)
	{
		XMStoreFloat4(&m_vColor, XMVectorLerp(vMiddleColor, vDstColor, (fHPRatio - 0.5f) * 2.f));
	}
	else
	{
		XMStoreFloat4(&m_vColor, XMVectorLerp(vSrcColor, vMiddleColor, fHPRatio * 2.f));
	}

	fHPRatio = fHPRatio * (0.83333f - 0.16666f) + 0.16666f;

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

HRESULT CHpBar_Player::Render()
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

HRESULT CHpBar_Player::Add_Components()
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

HRESULT CHpBar_Player::Bind_ShaderResources()
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

CHpBar_Player* CHpBar_Player::Create(_dev pDevice, _context pContext)
{
	CHpBar_Player* pInstance = new CHpBar_Player(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHpBar_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBar_Player::Clone(void* pArg)
{
	CHpBar_Player* pInstance = new CHpBar_Player(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHpBar_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Player::Free()
{
	__super::Free();

	Safe_Release(m_pFrameTextureCom);
	Safe_Release(m_pMaskTextureCom[0]);
	Safe_Release(m_pMaskTextureCom[1]);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
