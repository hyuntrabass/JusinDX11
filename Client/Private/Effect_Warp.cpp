#include "Effect_Warp.h"

CEffect_Warp::CEffect_Warp(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Warp::CEffect_Warp(const CEffect_Warp& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEffect_Warp::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Warp::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (pArg)
	{
		m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(reinterpret_cast<_float3*>(pArg)), 1.f));
	}

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(State::Pos));
	LightDesc.vAttenuation = LIGHT_RANGE_65;
	LightDesc.vDiffuse = _float4(1.f, 0.35f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

	m_pGameInstance->Delete_Light(LEVEL_CLOUD, TEXT("Light_Kurama_Warp"));
	m_pGameInstance->Add_Light(LEVEL_CLOUD, TEXT("Light_Kurama_Warp"), LightDesc);

	return S_OK;
}

void CEffect_Warp::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update(fTimeDelta);
}

void CEffect_Warp::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CEffect_Warp::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(InstPass_Particle_Color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Warp::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	CVIBuffer_Instancing::ParticleDesc Desc{};
	Desc.vMinPos = _float3(-7.f, 0.f, -7.f);
	Desc.vMaxPos = _float3(7.f, 0.f, 7.f);
	Desc.vScaleRange = _float2(0.2f, 0.5f);
	Desc.vSpeedRange = _float2(0.1f, 25.f);
	Desc.vLifeTime = _float2(1.f, 2.5f);
	Desc.vMinDir = _float3(0.f, 1.f, 0.f);
	Desc.vMaxDir = Desc.vMinDir;
	Desc.isLoop = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere01"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Warp::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View)))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	//_float4 vColor{ 0.f, 1.f, 0.8f, 1.f };
	_float4 vColor{ 1.f, 0.35f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEffect_Warp* CEffect_Warp::Create(_dev pDevice, _context pContext)
{
	CEffect_Warp* pInstance = new CEffect_Warp(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Warp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Warp::Clone(void* pArg)
{
	CEffect_Warp* pInstance = new CEffect_Warp(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Warp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Warp::Free()
{
	m_pGameInstance->Delete_Light(LEVEL_CLOUD, TEXT("Light_Kurama_Warp"));

	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}
