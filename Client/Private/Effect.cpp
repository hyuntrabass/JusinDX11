#include "Effect.h"

CEffect::CEffect(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
    : CGameObject(rhs)
{
}

HRESULT CEffect::Init_Prototype()
{
    return S_OK;
}

HRESULT CEffect::Init(void* pArg)
{
    if (FAILED(Add_Components()))
    {
        return E_FAIL;
    }

	m_pTransformCom->Set_State(State::Pos, XMVectorSet(17.f, 45.f, 140.f, 1.f));

    return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	//m_pTransformCom->LookAt(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));

	m_pVIBufferCom->Update(fTimeDelta);
}

void CEffect::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pTransformCom->LookAt_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f));

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	return S_OK;
}

HRESULT CEffect::Add_Components()
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
	Desc.vMaxPos = _float3(0.f, 0.f, 0.f);
	Desc.vScaleRange = _float2(0.05f, 0.1f);
	Desc.vSpeedRange = _float2(0.2f, 10.f);
	Desc.vLifeTime = _float2(1.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View)))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEffect* CEffect::Create(_dev pDevice, _context pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect");
		Safe_Release(pInstance);
	}

    return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect");
		Safe_Release(pInstance);
	}

    return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
