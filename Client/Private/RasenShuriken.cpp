#include "RasenShuriken.h"

CRasenShuriken::CRasenShuriken(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CRasenShuriken::CRasenShuriken(const CRasenShuriken& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CRasenShuriken::Init_Prototype()
{
	return S_OK;
}

HRESULT CRasenShuriken::Init(void* pArg)
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

	m_pTransformCom->Set_RotationPerSec(360.f);
	m_pTransformCom->Set_Speed(50.f);

	m_fDissolveRatio = 1.f;

	m_fCoreScale = 1.f;

	return S_OK;
}

void CRasenShuriken::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CRasenShuriken::State_Charge:
		m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(m_pPos) + XMVectorSet(0.f, 0.4f, 0.f, 0.f), 1.f));
		m_fCoreScale = Lerp(1.5f, 0.5f, m_fTimer);
		m_fCoreAlpha = Lerp(0.f, 0.7f, m_fTimer);
		m_fDissolveRatio -= fTimeDelta * 1.5f;
		break;
	case Client::CRasenShuriken::State_Shoot:
		m_fScaleRatio = {};
		if (m_hasTarget)
		{
			if (m_pTransformCom->Go_To(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f), fTimeDelta))
			{
				m_eState = State_Explode;
			}

			m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));
		}
		else
		{
			m_pTransformCom->Go_To_Dir(XMLoadFloat3(&m_vTargetPos), fTimeDelta);
			if (m_fTimer > 1.f)
			{
				m_eState = State_Explode;
			}
			m_pTransformCom->LookAt_Dir(XMLoadFloat3(&m_vTargetPos));
		}

		if (m_pGameInstance->CheckCollision_Monster(m_pColliderCom))
		{
			m_eState = State_Explode;
		}
		break;
	case Client::CRasenShuriken::State_Explode:
		//if (m_iAttCount == 0)
		{
		}
		m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
		m_fCoreScale = Lerp(0.5f, 10.5f, m_fScaleRatio);
		m_fCoreAlpha = Lerp(0.7f, 0.2f, m_fTimer);
		m_fScaleRatio += fTimeDelta;

		if (m_fTimer > 0.25)
		{
			m_pGameInstance->Set_ShakeCam(true);
			m_pGameInstance->Attack_Monster(m_pColliderCom, 13);
			m_fTimer = {};
			m_iAttCount++;
		}

		if (m_iAttCount >= 8)
		{
			m_eState = State_Dissolve;
			m_fScaleRatio = {};
			m_fDissolveRatio = {};
		}
		break;
	case Client::CRasenShuriken::State_Dissolve:
		m_fDissolveRatio += fTimeDelta / 2.5f;
		//m_fCoreScale = Lerp(10.5f, 20.5f, m_fScaleRatio);
		//m_fScaleRatio += fTimeDelta;
		if (m_fDissolveRatio > 1.f)
		{
			m_isDead = true;
		}
		break;
	}

	m_fTimer += fTimeDelta;


	m_vUVTransform.x += fTimeDelta * 4;
	if (m_vUVTransform.x > 2.f)
	{
		m_vUVTransform.x = 1.f;
	}

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	m_pColliderCom->Update(XMMatrixScaling(m_fCoreScale, m_fCoreScale, m_fCoreScale) * m_pTransformCom->Get_World_Matrix());
}

void CRasenShuriken::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_BlendBlur, this);
#ifdef _DEBUGG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CRasenShuriken::Render()
{
	_float4 vColor{};
	_uint iDissolve{};
	if (m_eState == State_Dissolve or m_eState == State_Charge)
	{
		iDissolve = 1;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof(_float))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		{
			return E_FAIL;
		}
	}

#pragma region Core
	_float44 World{};

#pragma region CoreCore
	XMStoreFloat4x4(&World, XMMatrixScaling(m_fCoreScale - 0.01f, m_fCoreScale - 0.01f, m_fCoreScale - 0.01f) * m_pTransformCom->Get_World_Matrix());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{
		return E_FAIL;
	}

	vColor = { 0.25f * 1.5f, 0.45f * 1.5f, 0.5f * 1.5f, m_fCoreAlpha };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[1]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion


	XMStoreFloat4x4(&World, XMMatrixScaling(m_fCoreScale, m_fCoreScale, m_fCoreScale) * m_pTransformCom->Get_World_Matrix());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{
		return E_FAIL;
	}

	vColor = { 0.4f * 1.5f, 0.5f * 1.5f, 0.5f * 1.5f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCoreMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUVTransform, sizeof(_float2))))
	{
		return E_FAIL;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[0]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion

	if (m_eState == State_Dissolve)
	{
		return S_OK;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

#pragma region Wing
	vColor = { 0.2f * 1.5f, 0.35f * 1.5f, 0.5f * 1.5f, 0.7f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[2]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion


#pragma region Wing Trail
	vColor = { 0.25f * 1.5f, 0.45f * 1.5f, 0.5f * 1.5f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pWingTrailMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[3]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion


#pragma region Circle
	vColor = { 0.25f, 0.45f, 0.5f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCircleMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[4]->Render(0)))
	{
		return E_FAIL;
	}

	vColor = { 0.f, 0.6f, 1.f, 0.1f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx + iDissolve)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[4]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

void CRasenShuriken::Shoot()
{
	m_eState = State_Shoot;
	m_fTimer = {};

	PxRaycastBuffer Buffer{};
	if (m_pGameInstance->Raycast(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()), XMLoadFloat4(&m_pGameInstance->Get_CameraLook()), 50.f, Buffer))
	{
		m_vTargetPos = _float3(Buffer.block.position.x, Buffer.block.position.y, Buffer.block.position.z);
		m_hasTarget = true;
	}
	else
	{
		m_vTargetPos = _float3(reinterpret_cast<_float*>(&m_pGameInstance->Get_CameraLook()));
		m_hasTarget = false;
	}

}

HRESULT CRasenShuriken::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere_HorizentalUV"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere_HorizentalUV"), TEXT("Com_Model_1_1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_RasenShuriken01"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_RasenShuriken02"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pModelCom[3]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_RasenShuriken03"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pModelCom[4]))))
	{
		return E_FAIL;
	}

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_RasenShuriken04"), TEXT("Com_Model_5"), reinterpret_cast<CComponent**>(&m_pModelCom[4]))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_Line_03"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pCircleMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_Rasengan_04"), TEXT("Com_CoreMaskTexture"), reinterpret_cast<CComponent**>(&m_pCoreMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gradient_Wave_03"), TEXT("Com_WingTrailMaskTexture"), reinterpret_cast<CComponent**>(&m_pWingTrailMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_Line_05"), TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
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

	return S_OK;
}

HRESULT CRasenShuriken::Bind_ShaderResources()
{
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

CRasenShuriken* CRasenShuriken::Create(_dev pDevice, _context pContext)
{
	CRasenShuriken* pInstance = new CRasenShuriken(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRasenShuriken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRasenShuriken::Clone(void* pArg)
{
	CRasenShuriken* pInstance = new CRasenShuriken(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRasenShuriken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasenShuriken::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}
	Safe_Release(m_pCircleMaskTextureCom);
	Safe_Release(m_pCoreMaskTextureCom);
	Safe_Release(m_pWingTrailMaskTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pColliderCom);
}
