#include "FootEffect.h"

CFootEffect::CFootEffect(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CFootEffect::CFootEffect(const CFootEffect& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CFootEffect::Init_Prototype()
{
	return S_OK;
}

HRESULT CFootEffect::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(_float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

void CFootEffect::Tick(_float3 vPos, _float fTimeDelta)
{
	if (m_TrailPosList.size() >= 20)
	{
		m_TrailPosList.pop_back();
	}
	m_TrailPosList.push_front(vPos);

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	m_pTransformCom->LookAway(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));
}

void CFootEffect::Late_Tick(_float fTimeDelta)
{
	_float3 PosArray[20]{};
	_float4 ColorArray[20]{};

	for (size_t i = 0; i < 20; i++)
	{
		XMStoreFloat3(&PosArray[i], m_pTransformCom->Get_State(State::Pos));
		ColorArray[i] = _float4(0.2f, 0.2f, 1.f, 1.f - static_cast<_float>(i) / 20.f);
	}

	_uint iIndex{};
	for (auto& vPos : m_TrailPosList)
	{
		PosArray[iIndex++] = vPos;
	}

	m_pTrailBufferCom->Update(PosArray, ColorArray);

	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
}

HRESULT CFootEffect::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_TrailShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailBufferCom->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(Bind_LightShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLightShaderCom->Begin(VTPass_Mask_Color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLightBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFootEffect::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_LightBuffer"), reinterpret_cast<CComponent**>(&m_pLightBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_LightShader"), reinterpret_cast<CComponent**>(&m_pLightShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere02"), TEXT("Com_EffectMask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_20"), TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail"), TEXT("Com_TrailShader"), reinterpret_cast<CComponent**>(&m_pTrailShaderCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFootEffect::Bind_TrailShaderResources()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFootEffect::Bind_LightShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pLightShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLightShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLightShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pLightShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	_float4 vColor{ 0.2f, 0.2f, 1.f, 1.f };
	//XMStoreFloat4(&vColor, Colors::CornflowerBlue);
	if (FAILED(m_pLightShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CFootEffect* CFootEffect::Create(_dev pDevice, _context pContext)
{
	CFootEffect* pInstnace = new CFootEffect(pDevice, pContext);

	if (FAILED(pInstnace->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFootEffect");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

CGameObject* CFootEffect::Clone(void* pArg)
{
	CFootEffect* pInstance = new CFootEffect(*this);

	if (FAILED(pInstance->Init((pArg))))
	{
		MSG_BOX("Failed to Clone : CFootEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFootEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pLightBufferCom);
	Safe_Release(m_pLightShaderCom);
	Safe_Release(m_pMaskTextureCom);

	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pTrailShaderCom);
}
