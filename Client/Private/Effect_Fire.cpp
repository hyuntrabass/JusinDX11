#include "Effect_Fire.h"

CEffect_Fire::CEffect_Fire(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Fire::CEffect_Fire(const CEffect_Fire& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Fire::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Fire::Init(void* pArg)
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

void CEffect_Fire::Tick(_float fTimeDelta)
{
	m_pTransformCom->LookAway(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));
	m_iSpriteIndex = 30 * m_fLifeTime;

	if (m_iSpriteIndex > 30)
	{
		m_fDissolveRatio += fTimeDelta;
	}

	m_fLifeTime += fTimeDelta;
	if (m_iSpriteIndex >= 64)
	{
		m_iSpriteIndex = 0;
		m_fLifeTime = {};
		m_isDead = true;
	}
	m_pVIBufferCom->Update(fTimeDelta, 50);
}

void CEffect_Fire::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonLight, this);
	m_pRendererCom->Add_RenderGroup(RG_Blur, this);
}

HRESULT CEffect_Fire::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(InstPass_Particle_Sprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBigDiffTextureCom->Bind_ShaderResource(m_pRectShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBigMaskTextureCom->Bind_ShaderResource(m_pRectShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pRectShaderCom->Bind_RawValue("g_iIndex", &m_iSpriteIndex, sizeof m_iSpriteIndex)))
	{
		return E_FAIL;
	}

	_int2 vNumSprite{ 8, 8 };
	if (FAILED(m_pRectShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof vNumSprite)))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pRectShaderCom->Begin(VTPass_Sprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferRectCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Fire::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_RectShader"), reinterpret_cast<CComponent**>(&m_pRectShaderCom))))
	{
		return E_FAIL;
	}

	CVIBuffer_Instancing::ParticleDesc Desc{};
	Desc.vMinPos = _float3(-2.f, -2.f, -2.f);
	Desc.vMaxPos = _float3(2.f, 2.f, 2.f);
	Desc.vScaleRange = _float2(0.2f, 1.f);
	Desc.vSpeedRange = _float2(0.f, 1.f);
	Desc.vLifeTime = _float2(10.f, 10.f);
	Desc.vMinDir = _float3(-1.f, 1.f, -1.f);
	Desc.vMaxDir = _float3(1.f, 1.f, 1.f);
	Desc.isLoop = true;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBufferRect"), reinterpret_cast<CComponent**>(&m_pVIBufferRectCom), &Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire_05"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_21"), TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_11"), TEXT("Com_BigMaskTexture"), reinterpret_cast<CComponent**>(&m_pBigMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_12"), TEXT("Com_BigDiffMaskTexture"), reinterpret_cast<CComponent**>(&m_pBigDiffTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Fire::Bind_ShaderResources()
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

	_float44 World{};
	XMStoreFloat4x4(&World, XMMatrixScaling(7.f, 7.f, 7.f) * m_pTransformCom->Get_World_Matrix());
	if (FAILED(m_pRectShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View)))
		|| FAILED(m_pRectShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj)))
		|| FAILED(m_pRectShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{
		return E_FAIL;
	}

	_float4 vColor{ 1.f, 0.4f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iSpriteIndex, sizeof m_iSpriteIndex)))
	{
		return E_FAIL;
	}

	_int2 vNumSprite{ 8, 8 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof vNumSprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof m_fDissolveRatio)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEffect_Fire* CEffect_Fire::Create(_dev pDevice, _context pContext)
{
	CEffect_Fire* pInstance = new CEffect_Fire(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Fire::Clone(void* pArg)
{
	CEffect_Fire* pInstance = new CEffect_Fire(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Fire::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRectShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pBigMaskTextureCom);
	Safe_Release(m_pBigDiffTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferRectCom);
}
