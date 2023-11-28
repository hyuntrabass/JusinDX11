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

	m_vAppearPoints[0] = _float3(-25.85f, 33.3f, -48.77f);
	m_vAppearPoints[1] = _float3(-17.4f, 73.f, -15.f);
	m_vAppearPoints[2] = _float3(14.8f, 65.f, 29.6f);
	m_vAppearPoints[3] = _float3(44.f, 33.f, 18.5f);
	m_vAppearPoints[4] = _float3(108.f, 51.f, -9.9f);

	_vector WarpPos = XMLoadFloat3(&m_vAppearPoints[m_iPosIndex]);
	WarpPos.m128_f32[3] = 1.f;

	m_pTransformCom->Set_State(State::Pos, WarpPos);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

	m_iHP = 500;

	return S_OK;
}

void CKurama::Tick(_float fTimeDelta)
{
	Artificial_Intelligence(fTimeDelta);

	Init_State();
	TIck_State(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

	m_pCollider_Hit->Update(m_pTransformCom->Get_World_Matrix());
	m_fTimer += fTimeDelta;
}

void CKurama::Late_Tick(_float fTimeDelta)
{
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

	m_pModelCom->Play_Animation(fTimeDelta);
	
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_eState != State_None)
	{
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

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
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

void CKurama::Set_Damage(_int iDamage)
{
	m_iHP -= iDamage;

	if (m_eState != State_MiniBomb and
		m_eState != State_ComboAttack)
	{
		m_eState = State_Beaten;
	}

	if (m_iPosIndex < 4)
	{
		m_iPosIndex++;
		m_eState = State_Warp;
	}
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

	if (FAILED(__super::Add_Component(LEVEL_BOSSSTAGE, TEXT("Prototype_Model_Kurama"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_BOSSSTAGE, 0);
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
	_float fDistToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

	m_pTransformCom->LookAt(vPlayerPosForLookAt);
	if (not m_hasInitiated)
	{
		if (m_fTimer > 3.f)
		{
			m_hasInitiated = true;
			m_eState = State_Initiation;
			m_fTimer = {};
		}
		else
		{
			return;
		}
	}

	if (m_iPosIndex < 4 and m_fTimer > 5.f and m_eState != State_Warp)
	{
		m_eState = State_MiniBomb;
		m_fTimer = {};
	}
	else if (m_fTimer > 5.f)
	{
		if (fDistToPlayer < 10.f)
		{
			m_eState = State_ComboAttack;
		}
		else
		{
			m_eState = State_MiniBomb;
		}
		m_fTimer = {};
	}
}

void CKurama::Init_State()
{
	if (m_eState != m_ePrevState)
	{
		ANIM_DESC Anim{};
		switch (m_eState)
		{
		case Client::CKurama::State_Initiation:
			Anim.iAnimIndex = Anim_etc_Appearance;
			Anim.bSkipInterpolation = true;

			m_pModelCom->Set_Animation(Anim);
			break;
		case Client::CKurama::State_Idle:
			Anim.iAnimIndex = Anim_Idle_Loop;
			Anim.isLoop = true;
			Anim.bSkipInterpolation = true;

			m_pModelCom->Set_Animation(Anim);
			break;
		case Client::CKurama::State_Attack:
			break;
		case Client::CKurama::State_ComboAttack:
			Anim.iAnimIndex = Anim_Attack_MowDown_Right;
			Anim.fAnimSpeedRatio = 3.5f;
			Anim.bSkipInterpolation = true;
			Anim.fDurationRatio = 0.5f;
			m_pModelCom->Set_Animation(Anim);
			break;
		case Client::CKurama::State_MiniBomb:
			m_fTimer = {};
			break;
		case Client::CKurama::State_Roar:
			break;
		case Client::CKurama::State_Warp:
			Anim.iAnimIndex = Anim_HandSeal_RecoveryChakra_Start;

			m_pModelCom->Set_Animation(Anim);
			break;
		case Client::CKurama::State_Beaten:
			break;
		case Client::CKurama::State_Die:
			break;
		}
		m_ePrevState = m_eState;
	}
}

void CKurama::TIck_State(_float fTimeDelta)
{
	ANIM_DESC Anim{};

	switch (m_eState)
	{
	case Client::CKurama::State_Initiation:
		if (m_pModelCom->IsAnimationFinished(Anim_etc_Appearance))
		{
			Anim.iAnimIndex = Anim_Ninjutsu_Roar;
			m_pModelCom->Set_Animation(Anim);
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Ninjutsu_Roar))
		{
			m_hasInitiated = true;
			m_eState = State_Idle;
		}
		break;
	case Client::CKurama::State_Idle:
		break;
	case Client::CKurama::State_Attack:
		break;
	case Client::CKurama::State_ComboAttack:
		Anim.fAnimSpeedRatio = 3.5f;
		Anim.bSkipInterpolation = true;
		Anim.fDurationRatio = 0.5f;

		if (m_pModelCom->IsAnimationFinished(Anim_Attack_MowDown_Right))
		{
			Anim.iAnimIndex = Anim_Attack_MowDown_Left;
			m_pModelCom->Set_Animation(Anim);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_MowDown_Left))
		{
			Anim.iAnimIndex = Anim_Attack_KickUp;
			m_pModelCom->Set_Animation(Anim);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_KickUp))
		{
			Anim.iAnimIndex = Anim_Attack_DoubleSledgehammer;
			Anim.fDurationRatio = 1.f;
			m_pModelCom->Set_Animation(Anim);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_DoubleSledgehammer))
		{
			m_eState = State_Idle;
		}

		m_fTimer = {};
		break;
	case Client::CKurama::State_MiniBomb:
		Anim.iAnimIndex = Anim_Ninjutsu_ConsecutiveTailedBeastBomb;
		m_pModelCom->Set_Animation(Anim);

		if (m_fTimer * 24.f > 22.f and m_fTimer * 24.f < 23.f or
			m_fTimer * 24.f > 32.f and m_fTimer * 24.f < 33.f or
			m_fTimer * 24.f > 42.f and m_fTimer * 24.f < 43.f)
		{
			if (not m_hasShot)
			{
				ObjectInfo Info{};
				XMStoreFloat4(&Info.vPos, XMVector4Transform(XMLoadFloat4x4(m_pModelCom->Get_pBoneMatrix("LipMouthDownCenter")).r[3], m_pTransformCom->Get_World_Matrix()));
				CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
				XMStoreFloat4(&Info.vLook, pPlayerTransform->Get_CenterPos());
				m_pGameInstance->Add_Layer(LEVEL_BOSSSTAGE, TEXT("Layer_MiniBomb"), TEXT("Prototype_GameObject_MiniBomb"), &Info);
				m_hasShot = true;
			}
		}
		else
		{
			m_hasShot = false;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Ninjutsu_ConsecutiveTailedBeastBomb))
		{
			m_fTimer = {};
			m_eState = State_Idle;
		}

		break;
	case Client::CKurama::State_Roar:
		break;
	case Client::CKurama::State_Warp:
		if (m_pModelCom->IsAnimationFinished(Anim_HandSeal_RecoveryChakra_Start))
		{
			Anim.iAnimIndex = Anim_HandSeal_RecoveryChakra_Loop;
			Anim.isLoop = true;
			m_fTimer = {};
			m_pModelCom->Set_Animation(Anim);
		}

		if (m_fTimer > 4.f and m_pModelCom->Get_CurrentAnimationIndex() == Anim_HandSeal_RecoveryChakra_Loop)
		{
			m_pTransformCom->Set_Position(m_vAppearPoints[m_iPosIndex]);
			Anim.iAnimIndex = Anim_HandSeal_RecoveryChakra_End;
			m_pModelCom->Set_Animation(Anim);
		}

		if (m_pModelCom->IsAnimationFinished(Anim_HandSeal_RecoveryChakra_End))
		{
			m_eState = State_Idle;
		}
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
