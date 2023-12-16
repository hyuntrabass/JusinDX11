#include "Chidori.h"

CChidori::CChidori(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CChidori::CChidori(const CChidori& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CChidori::Init_Prototype()
{
	return S_OK;
}

HRESULT CChidori::Init(void* pArg)
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

	m_pPos = reinterpret_cast<_float3*>(pArg);

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(m_pPos), 1.f));

	return S_OK;
}

void CChidori::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(m_pPos), 1.f));

	m_vUVTransform.x -= fTimeDelta * 8.f;
	if (m_vUVTransform.x < -2.f)
	{
		m_vUVTransform.x = 1.f;
	}
}

void CChidori::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
}

HRESULT CChidori::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < iNumModels; i++)
	{
		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom[i]->Render(0)))
		{
			return E_FAIL;
		}
	};

	return S_OK;
}

HRESULT CChidori::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningC1"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB1"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB2"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB3"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pModelCom[3]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB4"), TEXT("Com_Model_5"), reinterpret_cast<CComponent**>(&m_pModelCom[4]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_6"), reinterpret_cast<CComponent**>(&m_pModelCom[5]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_7"), reinterpret_cast<CComponent**>(&m_pModelCom[6]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_8"), reinterpret_cast<CComponent**>(&m_pModelCom[7]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lightning06"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CChidori::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUVTransform, sizeof(_float2))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CChidori* CChidori::Create(_dev pDevice, _context pContext)
{
	CChidori* pInstance = new CChidori(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChidori::Clone(void* pArg)
{
	CChidori* pInstance = new CChidori(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChidori::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < iNumModels; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}
	Safe_Release(m_pMaskTextureCom);
}
