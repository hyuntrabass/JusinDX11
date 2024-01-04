#include "Chidori.h"

CChidori::CChidori(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CChidori::CChidori(const CChidori& rhs)
	: CBlendObject(rhs)
{
}

void CChidori::Set_RushingState(const _bool isRushing)
{
	m_isRushing = isRushing;
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

	m_pSocketMatrix = reinterpret_cast<_float44*>(pArg);

	LIGHT_DESC* pMainLight_Desc = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"));
	m_OriginMainLight = *pMainLight_Desc;

	LIGHT_DESC* pPlayerLightDesc{};
	pPlayerLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Player"));
	m_OriginPlayerLight = *pPlayerLightDesc;
	pPlayerLightDesc->vDiffuse = _float4();

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	LightDesc.vAttenuation = LIGHT_RANGE_20;
	LightDesc.vDiffuse = _float4(0.3f, 0.7f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Chidori"), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CChidori::Tick(_float fTimeDelta)
{
	LIGHT_DESC* LightDesc{};
	LightDesc = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"));
	XMStoreFloat4(&LightDesc->vDiffuse, XMVectorLerp(XMLoadFloat4(&m_OriginMainLight.vDiffuse), XMVectorSet(0.2f, 0.2f, 0.2f, 1.f), clamp(m_fTimer * 2.f, 0.f, 1.f)));

	LightDesc = {};
	LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Chidori"));
	XMStoreFloat4(&LightDesc->vDiffuse, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.3f, 0.7f, 1.f, 1.f), clamp(m_fTimer * 2.f, 0.f, 1.f)));

	m_vUVTransform.x -= fTimeDelta * 4.f;
	m_vUVTransform.y -= fTimeDelta * 4.f;
	m_vUVTransform.z -= fTimeDelta * 4.f;
	if (m_vUVTransform.x < -2.f)
	{
		//m_pGameInstance->Set_ShakeCam(true);
		m_vUVTransform.x = 1.f;
		
		m_pGameInstance->StopSound(SCH_EFFECT_SKILL1);
		m_pGameInstance->Play_Sound(TEXT("Hitted_Chidori_2"), SCH_EFFECT_SKILL1);

	}
	if (m_vUVTransform.y < -2.f)
	{
		m_vUVTransform.y = 1.f;
		m_pGameInstance->StopSound(SCH_EFFECT_SKILL2);
		m_pGameInstance->Play_Sound(TEXT("Charge_Chidori_0"), SCH_EFFECT_SKILL2);
	}
	if (m_vUVTransform.z < -2.f)
	{
		m_vUVTransform.z = 1.f;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixTranslation(m_pSocketMatrix->_41, m_pSocketMatrix->_42, m_pSocketMatrix->_43) * m_pPlayerTransform->Get_World_Matrix());

	//if (m_fTraceTimer > 0.1f)
	if (m_isRushing)
	{
		_float3 vPos{ m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_LeftCidori"), TEXT("Prototype_GameObject_LeftChidori"), &vPos);
		m_fTraceTimer = {};
	}

	LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Chidori"));
	LightDesc->vPosition = _float4(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);

	m_fTraceTimer += fTimeDelta;
	m_fTimer += fTimeDelta;
}

void CChidori::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CChidori::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < m_iNumModels; i++)
	{
		if (i > 0 and static_cast<_float>(i) > m_fTimer * m_iNumModels)
		{
			break;
		}
		if (i == 5)
		{
			_float44 RotatedWorld{};
			XMStoreFloat4x4(&RotatedWorld, XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(100.f)) * XMLoadFloat4x4(&m_WorldMatrix));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", RotatedWorld)))
			{
				return E_FAIL;
			}

			//_float4 vColor{ 0.f, 0.f, 1.f, 1.f };
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			//{
			//	return E_FAIL;
			//}
		}

		if (i == 6)
		{
			_float44 RotatedWorld{};
			XMStoreFloat4x4(&RotatedWorld, XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(0.f)) * XMLoadFloat4x4(&m_WorldMatrix));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", RotatedWorld)))
			{
				return E_FAIL;
			}

			//_float4 vColor{ 1.f, 0.f, 0.f, 1.f };
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			//{
			//	return E_FAIL;
			//}
		}
		if (i == 7)
		{
			_float44 RotatedWorld{};
			XMStoreFloat4x4(&RotatedWorld, XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.3f, 0.f), XMConvertToRadians(90.f)) * XMLoadFloat4x4(&m_WorldMatrix));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", RotatedWorld)))
			{
				return E_FAIL;
			}

			//_float4 vColor{ 0.f, 1.f, 0.f, 1.f };
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			//{
			//	return E_FAIL;
			//}
		}
		if (i == 8)
		{
			_float44 RotatedWorld{};
			XMStoreFloat4x4(&RotatedWorld, XMMatrixScaling(1.3f, 1.3f, 1.3f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(70.f)) * XMLoadFloat4x4(&m_WorldMatrix));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", RotatedWorld)))
			{
				return E_FAIL;
			}

			//_float4 vColor{ 1.f, 1.f, 0.f, 1.f };
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			//{
			//	return E_FAIL;
			//}
		}

		_float2 vUVTransform{};
		if (i % 3 == 0)
		{
			vUVTransform.x = m_vUVTransform.x;
		}
		else if (i % 3 == 1)
		{
			vUVTransform.x = m_vUVTransform.y;
		}
		else if (i % 3 == 2)
		{
			vUVTransform.x = m_vUVTransform.z;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUVTransform, sizeof(_float2))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffectClamp)))
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

	_uint iModelIndex{};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB3"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB2"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB1"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB4"), TEXT("Com_Model_5"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningC1"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningC1"), TEXT("Com_Model_9"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_6"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_7"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Chidori"), TEXT("Com_Model_8"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_09"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

HRESULT CChidori::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
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
	LIGHT_DESC* pDesc = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"));
	*pDesc = m_OriginMainLight;
	pDesc = nullptr;

	pDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Player"));
	*pDesc = m_OriginPlayerLight;
	pDesc = nullptr;

	m_pGameInstance->Delete_Light(LEVEL_STATIC, TEXT("Light_Chidori"));

	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}
	Safe_Release(m_pMaskTextureCom);
}
