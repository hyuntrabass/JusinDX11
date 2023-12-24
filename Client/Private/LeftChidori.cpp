#include "LeftChidori.h"

CLeftChidori::CLeftChidori(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CLeftChidori::CLeftChidori(const CLeftChidori& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CLeftChidori::Init_Prototype()
{
	return S_OK;
}

HRESULT CLeftChidori::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_vPos = *reinterpret_cast<_float3*>(pArg);
	//m_iModelNumber = { 1 };
	m_iModelNumber = { rand() % m_iNumModels };

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&m_vPos), 1.f));
	random_device random;
	_randNum RandomNumber(random());
	_randFloat RandomAngle = _randFloat(0.f, 180.f);
	_randFloat RandomLook = _randFloat(0.f, 1.f);
	
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), RandomAngle(RandomNumber));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), RandomAngle(RandomNumber));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), RandomAngle(RandomNumber));
	m_pTransformCom->LookAt_Dir(XMVectorSet(RandomLook(RandomNumber), 0.f, RandomLook(RandomNumber), 0.f));
	
	return S_OK;
}

void CLeftChidori::Tick(_float fTimeDelta)
{
	m_fUVTransform -= fTimeDelta * 4.f;
	if (m_fUVTransform < -2.f)
	{
		m_fUVTransform = 1.f;
	}

	if (m_fTimer > 1.5f)
	{
		m_isDead = true;
	}

	m_fTimer += fTimeDelta;
}

void CLeftChidori::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CLeftChidori::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_float2 vUVTransform{};
	vUVTransform.x = m_fUVTransform;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUVTransform, sizeof(_float2))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffectClamp)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[m_iModelNumber]->Render(0)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLeftChidori::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	_uint iModelIndex{};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB1"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB3"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB2"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB4"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lightning06"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLeftChidori::Bind_ShaderResources()
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

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}
	//Colors::LightSkyBlue
	_float4 vColor{ 0.6f, 0.8f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLeftChidori* CLeftChidori::Create(_dev pDevice, _context pContext)
{
	CLeftChidori* pInstance = new CLeftChidori(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLeftChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLeftChidori::Clone(void* pArg)
{
	CLeftChidori* pInstance = new CLeftChidori(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLeftChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeftChidori::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}
	Safe_Release(m_pMaskTextureCom);
}
