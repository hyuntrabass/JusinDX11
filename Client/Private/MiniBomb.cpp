#include "MiniBomb.h"
#include "CommonTrail.h"

CMiniBomb::CMiniBomb(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CMiniBomb::CMiniBomb(const CMiniBomb& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CMiniBomb::Init_Prototype()
{
	return S_OK;
}

HRESULT CMiniBomb::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument");
		return E_FAIL;
	}

	ObjectInfo Info = *reinterpret_cast<ObjectInfo*>(pArg);

	//m_pTransformCom->Set_Scale(_float3(0.5f, 0.5f, 0.5f));
	Info.vPos.w = 1.f;
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&Info.vLook));

	m_pTransformCom->Set_Speed(100.f);
	m_pTransformCom->Set_RotationPerSec(360.f * 2.f);
	m_pGameInstance->Set_ShakeCam(true);

	TRAIL_DESC TrailDesc{};
	TrailDesc.vColor = _float4(0.7f, 0.f, 0.7f, 0.8f);
	TrailDesc.vPSize = _float2(1.f, 1.f);
	TrailDesc.iNumVertices = 5;

	random_device random;
	_randNum RandomNumber(random());
	_randFloat RandomX = _randFloat(0.f, 360.f);
	_randFloat RandomY = _randFloat(0.f, 360.f);
	_randFloat RandomZ = _randFloat(0.f, 360.f);

	for (size_t i = 0; i < m_iNumSparkModel; i++)
	{
		m_vAngle[i].x = RandomX(RandomNumber);
		m_vAngle[i].y = RandomY(RandomNumber);
		m_vAngle[i].z = RandomZ(RandomNumber);
	}

	m_pTrail = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));

	return S_OK;
}

void CMiniBomb::Tick(_float fTimeDelta)
{
	m_vSparkUVTransform.x -= fTimeDelta * 4.f;
	m_vSparkUVTransform.y -= fTimeDelta * 4.f;
	m_vSparkUVTransform.z -= fTimeDelta * 4.f;
	if (m_vSparkUVTransform.x < -2.f)
	{
		m_vSparkUVTransform.x = 1.f;
		m_bRotate[0] = true;
		m_bRotate[3] = true;
	}
	if (m_vSparkUVTransform.y < -2.f)
	{
		m_vSparkUVTransform.y = 1.f;
		m_bRotate[1] = true;
	}
	if (m_vSparkUVTransform.z < -2.f)
	{
		m_vSparkUVTransform.z = 1.f;
		m_bRotate[2] = true;
	}

	if (m_fLifeTimer > 1.f)
	{
		m_isDead = true;
		return;
	}

	m_fTrailTimer += fTimeDelta;

	if (m_fTrailTimer > 0.015f)
	{
		_float3 vPos{};
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
		m_pTrail->Tick(vPos);
		m_fTrailTimer = {};
	}

	//m_pTransformCom->Turn(XMVector3Normalize(m_pTransformCom->Get_State(State::Look) * -1.f), fTimeDelta);

	m_fLifeTimer += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta);

	m_pGameInstance->Attack_Player(m_pColliderCom, 15);
	if (m_pGameInstance->CheckCollision_Player(m_pColliderCom))
	{
		m_isDead = true;

		EffectInfo EffectInfo{};
		EffectInfo.vColor = _float4(0.175f, 0.175f, 0.35f, 1.f);
		EffectInfo.fScale = 10.f;
		XMStoreFloat4(&EffectInfo.vPos, m_pTransformCom->Get_State(State::Pos));
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &EffectInfo);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CMiniBomb::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);
	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CMiniBomb::Render()
{
	if (FAILED(Bind_Shader_Resources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCoreMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	m_pShaderCom->Begin(StaticPass_SingleColorFx);

	m_pModelCom->Render(0);

	if (FAILED(m_pSparkMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < m_iNumSparkModel; i++)
	{
		_float2 vUVTransform{};
		_float44 World{};

		if (i % 4 == 1 or i == 0)
		{
			vUVTransform.x = m_vSparkUVTransform.y;
			XMStoreFloat4x4(&World, XMMatrixScaling(1.8f, 1.8f, 1.8f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}
		else if (i % 4 == 2)
		{
			vUVTransform.x = m_vSparkUVTransform.z;
			XMStoreFloat4x4(&World, XMMatrixScaling(1.8f, 1.8f, 1.8f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}
		else
		{
			vUVTransform.x = m_vSparkUVTransform.x;
			XMStoreFloat4x4(&World, XMMatrixScaling(1.8f, 1.8f, 1.8f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUVTransform, sizeof(_float2))))
		{
			return E_FAIL;
		}

		_float4 vColor = { 0.f, 0.f, 0.f, 1.f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffectClamp)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pSparkModel[i]->Render(0)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMiniBomb::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 0.5f;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Noise_T_EFF_Noise_23_M"), TEXT("Com_CoreMaskTexture"), reinterpret_cast<CComponent**>(&m_pCoreMaskTexture))))
	{
		return E_FAIL;
	}

	_uint iModelIndex{};
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB1"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pSparkModel[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB3"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pSparkModel[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB2"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pSparkModel[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_LightningB4"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pSparkModel[iModelIndex++]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_09"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pSparkMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniBomb::Bind_Shader_Resources()
{
	_float4 vBaseColor{ 0.03f, 0.f, 0.5f, 0.8f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vBaseColor, sizeof _float4)))
	{
		return E_FAIL;
	}

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

	return S_OK;
}

CMiniBomb* CMiniBomb::Create(_dev pDevice, _context pContext)
{
	CMiniBomb* pInstance = new CMiniBomb(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMiniBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniBomb::Clone(void* pArg)
{
	CMiniBomb* pInstance = new CMiniBomb(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMiniBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniBomb::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumSparkModel; i++)
	{
		Safe_Release(m_pSparkModel[i]);
	}
	Safe_Release(m_pSparkMaskTextureCom);
	Safe_Release(m_pCoreMaskTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
