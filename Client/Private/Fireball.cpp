#include "Fireball.h"

CFireball::CFireball(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CFireball::CFireball(const CFireball& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CFireball::Init_Prototype()
{
	return S_OK;
}

HRESULT CFireball::Init(void* pArg)
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

	m_pTransformCom->Set_Speed(50.f);

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

	return S_OK;
}

void CFireball::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CFireball::State_Shoot:
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
	case Client::CFireball::State_Explode:
			m_pGameInstance->Attack_Monster(m_pColliderCom, 40);
			m_fTimer = {};
			_float3 vPos{};
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
			m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Effect"), TEXT("Prototype_GameObject_Effect_Fire"), &vPos);
			m_isDead = true;
		break;
	case Client::CFireball::State_Dissolve:
		break;
	}

	m_fTimer += fTimeDelta;


	m_vUVTransform.x += fTimeDelta;
	if (m_vUVTransform.x > 2.f)
	{
		m_vUVTransform.x = 1.f;
	}

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CFireball::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RG_NonLight, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CFireball::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCoreMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGradationTextureCom->Bind_ShaderResource(m_pShaderCom, "g_GradationTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_Fireball)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[0]->Render(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCircleMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUVTransform, sizeof(_float2))))
	{
		return E_FAIL;
	}

	_float4 vColor{ 1.f, 0.f, 0.f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[1]->Render(0)))
	{
		return E_FAIL;
	}

	vColor = { 1.f, 0.4f, 0.f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pWingTrailMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[2]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

void CFireball::Shoot()
{
	m_eState = State_Shoot;
	m_fTimer = {};
}

HRESULT CFireball::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Fireball_01"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Fireball_02"), TEXT("Com_Model_1_1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Fireball_03"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gradient_Wave_02"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pCircleMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_06"), TEXT("Com_CoreMaskTexture"), reinterpret_cast<CComponent**>(&m_pCoreMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire_07"), TEXT("Com_WingTrailMaskTexture"), reinterpret_cast<CComponent**>(&m_pWingTrailMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gradation_Fireball_03"), TEXT("Com_GradationTexture"), reinterpret_cast<CComponent**>(&m_pGradationTextureCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 2.f;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFireball::Bind_ShaderResources()
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

	return S_OK;
}

CFireball* CFireball::Create(_dev pDevice, _context pContext)
{
	CFireball* pInstance = new CFireball(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFireball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFireball::Clone(void* pArg)
{
	CFireball* pInstance = new CFireball(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CFireball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireball::Free()
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
	Safe_Release(m_pGradationTextureCom);
	Safe_Release(m_pColliderCom);
}
