#include "Effect_Smoke.h"

CEffect_Smoke::CEffect_Smoke(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Smoke::CEffect_Smoke(const CEffect_Smoke& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEffect_Smoke::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Smoke::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	EffectInfo Info = *reinterpret_cast<EffectInfo*>(pArg);

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
	m_vColor = Info.vColor;
	m_fScale = Info.fScale;
	m_iNumInstance = Info.iType;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Smoke::Tick(_float fTimeDelta)
{
	m_iSpriteIndex = 31 * m_fLifeTime;

	m_fLifeTime += fTimeDelta;
	if (m_iSpriteIndex >= 32)
	{
		m_iSpriteIndex = 0;
		m_fLifeTime = {};
		m_isDead = true;
	}
	m_pVIBufferCom->Update(fTimeDelta, m_iNumInstance);
}

void CEffect_Smoke::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonLight, this);
	//m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CEffect_Smoke::Render()
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

	return S_OK;
}

HRESULT CEffect_Smoke::Add_Components()
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
	Desc.vMinPos = _float3(-m_fScale, -m_fScale, -m_fScale);
	Desc.vMaxPos = _float3(m_fScale, m_fScale, m_fScale);
	Desc.vScaleRange = _float2(m_fScale * 0.25f, m_fScale);
	Desc.vSpeedRange = _float2(3.f, 5.f);
	Desc.vLifeTime = _float2(1.f, 1.f);
	Desc.vMinDir = _float3(-1.f, 2.f, -1.f);
	Desc.vMaxDir = _float3(1.f, 2.f, 1.f);
	Desc.isLoop = false;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_T_EFF_Smoke_09_M"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Smoke::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iSpriteIndex, sizeof m_iSpriteIndex)))
	{
		return E_FAIL;
	}

	_int2 vNumSprite{ 4, 8 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof vNumSprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
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

CEffect_Smoke* CEffect_Smoke::Create(_dev pDevice, _context pContext)
{
	CEffect_Smoke* pInstance = new CEffect_Smoke(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Smoke::Clone(void* pArg)
{
	CEffect_Smoke* pInstance = new CEffect_Smoke(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Smoke::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}
