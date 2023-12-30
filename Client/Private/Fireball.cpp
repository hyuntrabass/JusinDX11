#include "Fireball.h"

_uint CFireball::m_iLightIndex{};

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

	ObjectInfo Info = *reinterpret_cast<ObjectInfo*>(pArg);
	m_strType = Info.strPrototypeTag;

	if (m_strType == TEXT("Fireball"))
	{
		m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));

		m_pTransformCom->Set_Speed(50.f);

		PxRaycastBuffer Buffer{};
		if (m_pGameInstance->Raycast(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()), XMLoadFloat4(&m_pGameInstance->Get_CameraLook()), 50.f, Buffer))
		{
			m_vTargetPos = _float3(Buffer.block.position.x, Buffer.block.position.y, Buffer.block.position.z);
			m_hasTarget = true;
			m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));
		}
		else
		{
			m_vTargetPos = _float3(reinterpret_cast<_float*>(&m_pGameInstance->Get_CameraLook()));
			m_hasTarget = false;
		}

		m_pGameInstance->Set_ShakeCam(true);
	}
	else
	{
		_vector vPos = XMLoadFloat4(&Info.vPos) - XMVector3Normalize(XMLoadFloat4(&Info.vLook)) * 60.f;
		m_pTransformCom->Set_State(State::Pos, vPos);
		m_pTransformCom->Set_Speed(20.f);

		PxRaycastBuffer Buffer{};
		if (m_pGameInstance->Raycast(vPos, XMVector3Normalize(XMLoadFloat4(&Info.vLook)), 100.f, Buffer))
		{
			m_vTargetPos = _float3(Buffer.block.position.x, Buffer.block.position.y, Buffer.block.position.z);
			m_hasTarget = true;
			m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));
		}
		else
		{
			m_vTargetPos = _float3(Info.vLook.x, Info.vLook.y, Info.vLook.z);
			m_hasTarget = false;
		}

	}

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	LightDesc.vAttenuation = LIGHT_RANGE_32;
	LightDesc.vDiffuse = _float4(1.f, 0.3f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

	m_iMyLightIndex = m_iLightIndex++;
	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Fireball") + to_wstring(m_iMyLightIndex), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CFireball::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CFireball::State_Shoot:
		//m_pTransformCom->Set_RotationPerSec(360.f);
		//m_pTransformCom->Turn(XMVector3Normalize(m_pTransformCom->Get_State(State::Look)) * -1.f, fTimeDelta);
	{
		LIGHT_DESC* LightDesc{};
		LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Fireball") + to_wstring(m_iMyLightIndex));
		XMStoreFloat4(&LightDesc->vPosition, m_pTransformCom->Get_State(State::Pos));
	}

		if (m_hasTarget)
		{
			if (m_pTransformCom->Go_To(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f), fTimeDelta))
			{
				m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));
				m_eState = State_Explode;
			}
			else
			{
			}
		}
		else
		{
			m_pTransformCom->Go_To_Dir(XMLoadFloat3(&m_vTargetPos), fTimeDelta);
			if (m_strType == TEXT("Fireball"))
			{
				if (m_fTimer > 1.f)
				{
					m_eState = State_Explode;
				}
			}
			else
			{
				if (m_fTimer > 10.f)
				{
					m_eState = State_Explode;
				}
			}
			m_pTransformCom->LookAt_Dir(XMVector3Normalize(XMLoadFloat3(&m_vTargetPos)));
		}

		if (m_pGameInstance->CheckCollision_Monster(m_pColliderCom))
		{
			m_pTransformCom->Set_State(State::Pos, m_pTransformCom->Get_State(State::Pos) + XMVector3Normalize(m_pTransformCom->Get_State(State::Look)) * 2.f);
			m_eState = State_Explode;
		}
		break;
	case Client::CFireball::State_Explode:
	{
		m_pGameInstance->Set_ShakeCam(true);
		m_pGameInstance->Attack_Monster(m_pColliderCom, 30, DAM_FIRE);
		m_fTimer = {};
		_float3 vPos{};
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
		m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Effect"), TEXT("Prototype_GameObject_Effect_Fire"), &vPos);
		EffectInfo Info{};
		Info.vColor = _float4(1.f, 0.3f, 0.f, 1.f);
		Info.fScale = 20.f;
		Info.vPos = _float4(vPos.x, vPos.y, vPos.z, 1.f);
		Info.iType = 1;
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &Info);
		Info.vColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
		Info.fScale = 2.f;
		Info.vPos = _float4(vPos.x, vPos.y, vPos.z, 1.f);
		Info.iType = 70;
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Smoke"), &Info);

		m_isDead = true;
		break;
	}
	case Client::CFireball::State_Dissolve:
		break;
	}

	m_fTimer += fTimeDelta;


	_float fUVDeltaRatio = 1.f;
	if (m_strType == TEXT("Meteor"))
	{
		fUVDeltaRatio = 3.f;
	}
	m_vUVTransform.x += fTimeDelta * fUVDeltaRatio;
	if (m_vUVTransform.x > 2.f)
	{
		m_vUVTransform.x = 1.f;
	}

	_float fColliderScale{ 1.f };
	if (m_eState == State_Explode)
	{
		fColliderScale = 1.5f;
	}
	m_pColliderCom->Update(XMMatrixScaling(fColliderScale, fColliderScale, fColliderScale) * m_pTransformCom->Get_World_Matrix());
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

	_float4 vColor{};

	if (m_strType == TEXT("Fireball"))
	{
		vColor = { 1.f, 0.4f, 0.f, 1.f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
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

		if (FAILED(m_pCircleMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUVTransform, sizeof(_float2))))
		{
			return E_FAIL;
		}

		vColor = { 1.f, 0.4f, 0.4f, 0.5f };
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
	}
	else
	{
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUVTransform, sizeof(_float2))))
		{
			return E_FAIL;
		}
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
	m_pGameInstance->Delete_Light(LEVEL_STATIC, TEXT("Light_Fireball") + to_wstring(m_iMyLightIndex));

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
