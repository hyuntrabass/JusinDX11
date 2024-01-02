#include "Effect_Impact.h"

CEffect_Impact::CEffect_Impact(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Impact::CEffect_Impact(const CEffect_Impact& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEffect_Impact::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Impact::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	EffectInfo Info{ *reinterpret_cast<EffectInfo*>(pArg) };

	m_vColor = Info.vColor;
	m_fMaxSize = Info.fScale;
	m_iType = Info.iType;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(_float3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));

	return S_OK;
}

void CEffect_Impact::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * m_fMaxSize * 5.f;

	m_pTransformCom->LookAway(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));
	m_pTransformCom->Set_Scale(_float3(m_fTimeAcc, m_fTimeAcc, m_fTimeAcc));

	if (m_fTimeAcc > m_fMaxSize)
	{
		m_fAlpha -= fTimeDelta * 5.f;
	}
	if (m_fAlpha < 0.f)
	{
		m_isDead = true;
		m_fTimeAcc = {};
		m_fAlpha = { 1.f };
	}
}

void CEffect_Impact::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CEffect_Impact::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_ColorAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Impact::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBufferRect"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (m_iType == 0)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Effect_T_EFF_Ring_01_M"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		{
			return E_FAIL;
		}
	}
	else if (m_iType == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hit_T_EFF_Hit_02_M"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		{
			return E_FAIL;
		}
	}
	else if (m_iType == 2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Effect_T_EFF_Radiation_01_M"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Impact::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View)) ||
			   FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj)))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof m_fAlpha)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEffect_Impact* CEffect_Impact::Create(_dev pDevice, _context pContext)
{
	CEffect_Impact* pInstance = new CEffect_Impact(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Impact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Impact::Clone(void* pArg)
{
	CEffect_Impact* pInstance = new CEffect_Impact(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Impact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Impact::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
