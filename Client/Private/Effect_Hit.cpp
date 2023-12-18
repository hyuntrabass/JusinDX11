#include "Effect_Hit.h"

CEffect_Hit::CEffect_Hit(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Hit::CEffect_Hit(const CEffect_Hit& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Hit::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Hit::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (pArg)
	{
		m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(reinterpret_cast<_float3*>(pArg)), 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(17.f, 45.f, 140.f, 1.f));
	}

	return S_OK;
}

void CEffect_Hit::Tick(_float fTimeDelta)
{
	//m_pTransformCom->LookAway(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));

	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > 0.2f)
	{
		m_isDead = true;
	}
	m_pVIBufferCom->Update(fTimeDelta);
}

void CEffect_Hit::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonLight, this);
}

HRESULT CEffect_Hit::Render()
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

	//m_pTransformCom->LookAt_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f));

	//if (FAILED(Bind_ShaderResources()))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Begin(InstPass_Particle)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pVIBufferCom->Render()))
	//{
	//	return E_FAIL;
	//}
	//m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	return S_OK;
}

HRESULT CEffect_Hit::Add_Components()
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
	Desc.vMinPos = _float3(0.f, 0.f, 0.f);
	Desc.vMaxPos = Desc.vMinPos;
	Desc.vScaleRange = _float2(0.01f, 0.05f);
	Desc.vSpeedRange = _float2(1.f, 20.f);
	Desc.vLifeTime = _float2(0.2f, 0.2f);
	Desc.vMinDir = _float3(-1.f, -1.f, -1.f);
	Desc.vMaxDir = _float3(1.f, 1.f, 1.f);
	Desc.isLoop = false;

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

HRESULT CEffect_Hit::Bind_ShaderResources()
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

	_float4 vColor{ 1.f, 0.6f, 0.f, 1.f };
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

CEffect_Hit* CEffect_Hit::Create(_dev pDevice, _context pContext)
{
	CEffect_Hit* pInstance = new CEffect_Hit(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Hit::Clone(void* pArg)
{
	CEffect_Hit* pInstance = new CEffect_Hit(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hit::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}
