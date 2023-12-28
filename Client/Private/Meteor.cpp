#include "Meteor.h"

CMeteor::CMeteor(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CMeteor::Init_Prototype()
{
	return S_OK;
}

HRESULT CMeteor::Init(void* pArg)
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

	m_fCoreScale = 0.01f;

	return S_OK;
}

void CMeteor::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CMeteor::State_Charge:
		m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(m_pPos) + XMVectorSet(0.f, 0.1f, 0.f, 0.f), 1.f));
		m_fCoreScale = Lerp(0.01f, 0.05f, m_fTimer / 2.f);
		break;
	case Client::CMeteor::State_Shoot:
		if (m_hasTarget)
		{
			if (m_pTransformCom->Go_To(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f), fTimeDelta))
			{
				m_eState = State_Summon;
			}

			m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));
		}
		else
		{
			m_pTransformCom->Go_To_Dir(XMLoadFloat3(&m_vTargetPos), fTimeDelta);
			if (m_fTimer > 1.f)
			{
				m_eState = State_Summon;
			}
			m_pTransformCom->LookAt_Dir(XMLoadFloat3(&m_vTargetPos));
		}

		if (m_pGameInstance->CheckCollision_Monster(m_pColliderCom))
		{
			m_eState = State_Summon;
		}
		break;
	case Client::CMeteor::State_Summon:
		random_device random;
		_randNum RandomNumber(random());
		_randFloat RandomX = _randFloat(-10.f, 10.f);
		_randFloat RandomZ = _randFloat(-10.f, 10.f);
		_randFloat RandomLookX = _randFloat(-1.f, 1.f);
		_randFloat RandomLookZ = _randFloat(-1.f, 1.f);

		_float4 vCurrPos{};
		XMStoreFloat4(&vCurrPos, m_pTransformCom->Get_State(State::Pos));

		if (m_fTimer > 0.25f)
		{
			ObjectInfo Info{};
			Info.vPos = _float4(vCurrPos.x + RandomX(RandomNumber), vCurrPos.y, vCurrPos.z + RandomZ(RandomNumber), 1.f);
			Info.vLook = _float4(RandomLookX(RandomNumber), -3.f, RandomLookZ(RandomNumber), 0.f);
			Info.strPrototypeTag = TEXT("Meteor");
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Meteor"), TEXT("Prototype_GameObject_Fireball"), &Info);
			
			m_fTimer = {};
			m_iSummonCount++;
		}

		if (m_iSummonCount > 40)
		{
			m_isDead = true;
		}
		break;
	}

	m_fTimer += fTimeDelta;


	m_vUVTransform.x -= fTimeDelta * 4.f;
	m_vUVTransform.y -= fTimeDelta * 4.f;
	m_vUVTransform.z -= fTimeDelta * 4.f;
	if (m_vUVTransform.x < -2.f)
	{
		m_vUVTransform.x = 1.f;
		m_bRotate[0] = true;
		m_bRotate[3] = true;
	}
	if (m_vUVTransform.y < -2.f)
	{
		m_vUVTransform.y = 1.f;
		m_bRotate[1] = true;
	}
	if (m_vUVTransform.z < -2.f)
	{
		m_vUVTransform.z = 1.f;
		m_bRotate[2] = true;
	}

	m_pColliderCom->Update(XMMatrixScaling(m_fCoreScale, m_fCoreScale, m_fCoreScale) * m_pTransformCom->Get_World_Matrix());
}

void CMeteor::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_BlendBlur, this);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CMeteor::Render()
{
	_float4 vColor{};

#pragma region Core
	_float44 World{};

#pragma region CoreCore
	XMStoreFloat4x4(&World, XMMatrixScaling(m_fCoreScale, m_fCoreScale, m_fCoreScale) * m_pTransformCom->Get_World_Matrix());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{
		return E_FAIL;
	}

	vColor = { 0.3f, 0.f, 0.f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[0]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion


	XMStoreFloat4x4(&World, XMMatrixScaling(m_fCoreScale * 1.1f, m_fCoreScale * 1.1f, m_fCoreScale * 1.1f) * m_pTransformCom->Get_World_Matrix());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{
		return E_FAIL;
	}

	vColor = { 1.f, 0.4f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCoreMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	_float2 NoUVMove{};
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &NoUVMove, sizeof NoUVMove)))
	{
		return E_FAIL;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom[0]->Render(0)))
	{
		return E_FAIL;
	}
#pragma endregion

	if (FAILED(m_pEffectMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	random_device random;
	_randNum RandomNumber(random());
	_randFloat RandomX = _randFloat(0.f, 360.f);
	_randFloat RandomY = _randFloat(0.f, 360.f);
	_randFloat RandomZ = _randFloat(0.f, 360.f);

	for (size_t i = 0; i < (m_iNumModels - 1) * 2; i++)
	{
		if (m_bRotate[i % 4])
		{
			if (i % 4 == 1 or i == 0)
			{
				m_vAngle[i].x = RandomX(RandomNumber);
				m_vAngle[i].y = RandomY(RandomNumber);
				m_vAngle[i].z = RandomZ(RandomNumber);
				m_bRotate[i] = false;
			}
			else if (i % 4 == 2)
			{
				m_vAngle[i].x = RandomX(RandomNumber);
				m_vAngle[i].y = RandomY(RandomNumber);
				m_vAngle[i].z = RandomZ(RandomNumber);
				m_bRotate[i] = false;
			}
			else
			{
				m_vAngle[i].x = RandomX(RandomNumber);
				m_vAngle[i].y = RandomY(RandomNumber);
				m_vAngle[i].z = RandomZ(RandomNumber);
				m_bRotate[i] = false;
			}
		}

		_float2 vUVTransform{};
		if (i % 4 == 1 or i == 0)
		{
			vUVTransform.x = m_vUVTransform.y;
			XMStoreFloat4x4(&World, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}
		else if (i % 4 == 2)
		{
			vUVTransform.x = m_vUVTransform.z;
			XMStoreFloat4x4(&World, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}
		else
		{
			vUVTransform.x = m_vUVTransform.x;
			XMStoreFloat4x4(&World, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationX(XMConvertToRadians(m_vAngle[i].x)) * XMMatrixRotationY(XMConvertToRadians(m_vAngle[i].y)) * XMMatrixRotationZ(XMConvertToRadians(m_vAngle[i].z)) * m_pTransformCom->Get_World_Matrix());
		}

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUVTransform, sizeof(_float2))))
		{
			return E_FAIL;
		}

		vColor = { 1.f, 0.f, 0.f, 1.f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffectClamp)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom[(i % 4) + 1]->Render(0)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CMeteor::Shoot()
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

HRESULT CMeteor::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Fireball_01"), TEXT("Com_Model_0"), reinterpret_cast<CComponent**>(&m_pModelCom[iModelIndex++]))))
	{
		return E_FAIL;
	}

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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_06"), TEXT("Com_CoreMaskTexture"), reinterpret_cast<CComponent**>(&m_pCoreMaskTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_09"), TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pEffectMaskTextureCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 0.01f;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMeteor::Bind_ShaderResources()
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

CMeteor* CMeteor::Create(_dev pDevice, _context pContext)
{
	CMeteor* pInstance = new CMeteor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
	CMeteor* pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeteor::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (size_t i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}
	Safe_Release(m_pCoreMaskTextureCom);
	Safe_Release(m_pEffectMaskTextureCom);
	Safe_Release(m_pColliderCom);
}
