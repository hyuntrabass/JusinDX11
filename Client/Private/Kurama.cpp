#include "Kurama.h"

CKurama::CKurama(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CKurama::CKurama(const CKurama& rhs)
	: CGameObject(rhs)
{
}

HRESULT CKurama::Init_Prototype()
{
	return S_OK;
}

HRESULT CKurama::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pModelCom->Set_Animation(KuraDef_etc_Appearance, false, 1.f, true);

	m_vAppearPoints[0] = _float4(-6.5f, 47.5f, 111.7f, 1.f);
	m_vAppearPoints[1] = _float4(24.f, 31.f, 121.1f, 1.f);
	m_vAppearPoints[2] = _float4(45.5f, 42.5f, 85.2f, 1.f);
	m_vAppearPoints[3] = _float4(21.5f, 41.f, 11.1f, 1.f);
	m_vAppearPoints[4] = _float4(-4.5f, 36.1f, -56.f, 1.f);

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_vAppearPoints[1]));

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

	m_iHP = 500;

	return S_OK;
}

void CKurama::Tick(_float fTimeDelta)
{
	Artificial_Intelligence(fTimeDelta);
	Apply_State(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

	m_pCollider_Hit->Update(m_pTransformCom->Get_World_Matrix());
}

void CKurama::Late_Tick(_float fTimeDelta)
{
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

	//_vector vPos = ;
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_hasInitiated)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
	}
}

HRESULT CKurama::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	m_pCollider_Hit->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CKurama::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Model_Kurama"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 5.f;
	ColDesc.vCenter = _float3(0.f, ColDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pCollider_Hit), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKurama::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_TUTORIAL, 0);
	if (not pLightDesc)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CKurama::Artificial_Intelligence(_float fTimeDelta)
{
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	_vector vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vector vPlayerPosForLookAt = XMVectorSetY(vPlayerPos, vMyPos.m128_f32[1]);
	m_pTransformCom->LookAt(vPlayerPosForLookAt);

	if (not m_hasInitiated and 
		XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos)) < 30.f)
	{
		m_hasInitiated = true;
		m_eState = State_Initiation;
	}

	if (m_hasInitiated)
	{
		m_fTimer += fTimeDelta;
	}

	if (m_fTimer > 10.f)
	{
		m_eState = State_Bomb;
		m_fTimer = {};
	}
}

void CKurama::Apply_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CKurama::State_Initiation:
		if (m_pModelCom->IsAnimationFinished(KuraDef_etc_Appearance))
		{
			m_pModelCom->Set_Animation(KuraDef_Ninjutsu_Roar, false);
		}

		if (m_pModelCom->IsAnimationFinished(KuraDef_Ninjutsu_Roar))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CKurama::State_Idle:
		m_pModelCom->Set_Animation(KuraDef_Idle_Loop, true, 1.f, true);
		break;
	case Client::CKurama::State_Attack:
		break;
	case Client::CKurama::State_Bomb:
		m_pModelCom->Set_Animation(KuraDef_Ninjutsu_TailedBeastBomb, false);

		if (m_pModelCom->IsAnimationFinished(KuraDef_Ninjutsu_TailedBeastBomb))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CKurama::State_Roar:
		break;
	case Client::CKurama::State_Beaten:
		break;
	case Client::CKurama::State_Die:
		break;
	}
}

CKurama* CKurama::Create(_dev pDevice, _context pContext)
{
	CKurama* pInstance = new CKurama(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CKurama");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKurama::Clone(void* pArg)
{
	CKurama* pInstance = new CKurama(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CKurama");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKurama::Free()
{
	__super::Free();

	Safe_Release(m_pCollider_Hit);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
