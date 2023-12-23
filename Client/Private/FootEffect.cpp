#include "FootEffect.h"
#include "CommonTrail.h"

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

	TRAIL_DESC TrailDesc{};
	TrailDesc.vColor = _float4(0.2f, 0.2f, 1.f, 1.f);
	TrailDesc.vPSize = _float2(0.01f, 0.01f);
	TrailDesc.iNumVertices = 20;

	m_pTrail = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));

	m_pTransformCom->Set_Scale(_float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

void CFootEffect::Tick(_float3 vPos, _float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	m_pTransformCom->LookAway(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));

	m_fTrailTimer += fTimeDelta;

	if (m_fTrailTimer > 0.015f)
	{
		m_pTrail->Tick(vPos);
		m_fTrailTimer = {};
	}
}

void CFootEffect::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);

	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CFootEffect::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_LightBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_LightShader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere02"), TEXT("Com_EffectMask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFootEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	_float4 vColor{ 0.2f, 0.2f, 1.f, 1.f };
	//XMStoreFloat4(&vColor, Colors::CornflowerBlue);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
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

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);

	Safe_Release(m_pTrail);
}
