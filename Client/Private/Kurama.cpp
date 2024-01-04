#include "Kurama.h"
#include "CommonTrail.h"
#include "UI_Manager.h"
#include "Indicator.h"

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
	m_vAppearPoints[1] = _float3(-18.572247f, 73.f, -19.051219f);
	m_vAppearPoints[2] = _float3(14.8f, 65.f, 29.6f);
	m_vAppearPoints[3] = _float3(44.f, 33.f, 18.5f);
	m_vAppearPoints[4] = _float3(108.f, 51.f, -9.9f);

	_vector WarpPos = XMLoadFloat3(&m_vAppearPoints[m_iPosIndex]);
	WarpPos.m128_f32[3] = 1.f;

	m_pTransformCom->LookAt_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(State::Pos, WarpPos);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

	TRAIL_DESC TrailDesc{};

	TrailDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);
	TrailDesc.vPSize = _float2(0.01f, 0.01f);
	TrailDesc.iNumVertices = 50;
	m_pEyeLights[0] = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));
	m_pEyeLights[1] = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));

	TrailDesc.vColor = _float4(1.f, 0.35f, 0.f, 1.f);
	TrailDesc.vPSize = _float2(0.05f, 0.05f);
	TrailDesc.iNumVertices = 20;
	for (size_t i = 0; i < 10; i++)
	{
		m_pFingerLights[i] = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));
		m_pFingerLights[i]->Off();
	}

	m_iMaxHP = 500;
	m_iHP = m_iMaxHP;

	return S_OK;
}

void CKurama::Tick(_float fTimeDelta)
{
	Artificial_Intelligence(fTimeDelta);

	Init_State();
	Tick_State(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);
	m_pModelCom->Set_Animation(m_AnimationDesc);
	m_AnimationDesc.bRestartAnimation = false;

	_float3 vEyePos[2]{};
	XMStoreFloat3(&vEyePos[0], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("EyeUpLeft1")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vEyePos[1], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("EyeUpRight1")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	m_pEyeLights[0]->Tick(vEyePos[0]);
	m_pEyeLights[1]->Tick(vEyePos[1]);

	_float3 vFingerPos[10];
	XMStoreFloat3(&vFingerPos[0], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LeftHandThumb3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[1], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LeftHandIndex3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[2], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LeftHandMiddle3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[3], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LeftHandRing3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[4], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LeftHandPinky3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[5], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("RightHandThumb3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[6], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("RightHandIndex3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[7], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("RightHandMiddle3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[8], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("RightHandRing3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);
	XMStoreFloat3(&vFingerPos[9], (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("RightHandPinky3")) * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix()).r[3]);

	for (size_t i = 0; i < 10; i++)
	{
		m_pFingerLights[i]->Tick(vFingerPos[i]);
	}

	if (m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);
	}

	_matrix ColliderOffset = XMMatrixTranslation(0.f, 4.f, 0.f);
	m_pCollider_Hit->Update(ColliderOffset * m_pTransformCom->Get_World_Matrix());
	m_fTimer += fTimeDelta;

	if (m_pIndicator)
	{
		_float fCamDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()) - m_pTransformCom->Get_State(State::Pos)));

		_float3 v2DPos{};
		XMStoreFloat3(&v2DPos, XMVector3Project(m_pTransformCom->Get_State(State::Pos), 0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, m_pGameInstance->Get_Transform(TransformType::Proj), m_pGameInstance->Get_Transform(TransformType::View), XMMatrixTranslation(0.f, Lerp(15.f, 16.0f, fCamDist / 30.f), 0.f)));

		if (v2DPos.z > 1.f)
		{
			v2DPos = _float3(-1.f, -1.f, -1.f);
		}

		m_pIndicator->Tick(_float2(v2DPos.x, v2DPos.y));
	}

	if (m_eState != State_Die)
	{
		CUI_Manager::Get_Instance()->Set_HP(TEXT("Kurama"), m_iMaxHP, m_iHP);
	}
}

void CKurama::Late_Tick(_float fTimeDelta)
{
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pIndicator)
	{
		m_pIndicator->Late_Tick(fTimeDelta);
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_eState != State_None)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

		m_pEyeLights[0]->Late_Tick(fTimeDelta);
		m_pEyeLights[1]->Late_Tick(fTimeDelta);

		for (size_t i = 0; i < 10; i++)
		{
			m_pFingerLights[i]->Late_Tick(fTimeDelta);
		}

		if (m_pEffect)
		{
			m_pEffect->Late_Tick(fTimeDelta);
		}

	#ifdef _DEBUGG
		m_pRendererCom->Add_DebugComponent(m_pCollider_Hit);
	#endif // _DEBUG
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

		//if (FAILED(m_pShaderCom->Begin(AnimPass_OutLine)))
		//{
		//	return E_FAIL;
		//}

		//if (FAILED(m_pModelCom->Render(i)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CKurama::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (m_eState == State_Die)
	{
		return;
	}
	if (iDamageType == DAM_ELECTRIC)
	{
		if (m_hasTakenChidori)
		{
			return;
		}
		else
		{
			m_hasTakenChidori = true;
		}
	}
	m_iHP -= iDamage;
	if (m_iHP < 0)
	{
		m_iHP = 0;
	}

	if (iDamage > static_cast<_int>(m_iSuperArmor))
	{
		m_eState = State_Beaten;
		m_AnimationDesc = {};
		m_AnimationDesc.iAnimIndex = Anim_Beaten_Type01;
		m_AnimationDesc.bSkipInterpolation = true;
		m_AnimationDesc.bRestartAnimation = true;
		m_pModelCom->Set_Animation(m_AnimationDesc);
		m_AnimationDesc.bRestartAnimation = false;

		m_iSuperArmor = {};
	}
	CUI_Manager::Get_Instance()->Create_Hit();
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

	if (FAILED(__super::Add_Component(LEVEL_CLOUD, TEXT("Prototype_Model_Kurama"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CKurama::Artificial_Intelligence(_float fTimeDelta)
{
	if (m_eState == State_Beaten or
		m_eState == State_Warp or
		m_eState == State_Die)
	{
		return;
	}
	_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
	_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
	_float fDistToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

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

		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI_HpBar"), TEXT("Prototype_GameObject_UI_BossHpBar"));
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI_HpBar_Base"), TEXT("Prototype_GameObject_UI_BossHpBar_Base"));
	}

	if (m_iPosIndex < 4 and m_fTimer > 5.f)
	{
		m_eState = State_MiniBomb;
		m_fTimer = {};
	}
	else if (m_fTimer > 5.f)
	{
		if (fDistToPlayer < 13.f)
		{
			if (rand() % 2)
			{
				m_eState = State_ComboAttack;
			}
			else
			{
				m_eState = State_ComboAttack2;
			}
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
		m_AnimationDesc = {};
		switch (m_eState)
		{
		case Client::CKurama::State_Initiation:
			m_AnimationDesc.iAnimIndex = Anim_etc_Appearance;
			m_AnimationDesc.bSkipInterpolation = true;
			break;
		case Client::CKurama::State_Idle:
			m_AnimationDesc.iAnimIndex = Anim_Idle_Loop;
			m_AnimationDesc.isLoop = true;
			m_fTimer = {};
			m_iSuperArmor = {};
			for (size_t i = 0; i < 10; i++)
			{
				m_pFingerLights[i]->Off();
			}
			break;
		case Client::CKurama::State_LookAt:
			XMStoreFloat3(&m_vOriginalLook, m_pTransformCom->Get_State(State::Look));
			m_fTimer = {};
			break;
		case Client::CKurama::State_Attack:
			break;
		case Client::CKurama::State_ComboAttack:
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_MowDown_Right;
			m_AnimationDesc.fAnimSpeedRatio = 3.5f;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.fDurationRatio = 0.5f;
			_vector vPlayerPosForLookAt = XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos), m_pTransformCom->Get_State(State::Pos).m128_f32[1]);
			m_pTransformCom->LookAt(vPlayerPosForLookAt);

			m_iSuperArmor = 50;
			for (size_t i = 0; i < 10; i++)
			{
				m_pFingerLights[i]->On();
			}
			break;
		}
		case Client::CKurama::State_ComboAttack2:
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_SwingingDownScratch_Right;
			m_AnimationDesc.fAnimSpeedRatio = 3.5f;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.fDurationRatio = 0.5f;
			_vector vPlayerPosForLookAt = XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos), m_pTransformCom->Get_State(State::Pos).m128_f32[1]);
			m_pTransformCom->LookAt(vPlayerPosForLookAt);

			m_iSuperArmor = 50;
			for (size_t i = 0; i < 10; i++)
			{
				m_pFingerLights[i]->On();
			}
			break;
		}
		case Client::CKurama::State_MiniBomb:
			m_fTimer = {};

			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_Ninjutsu_ConsecutiveTailedBeastBomb;

			m_iSuperArmor = 12;
			break;
		case Client::CKurama::State_Roar:
			break;
		case Client::CKurama::State_Warp:
			m_AnimationDesc.iAnimIndex = Anim_HandSeal_RecoveryChakra_Start;

			Safe_Release(m_pEffect);
			_float3 vPos{};
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
			m_pEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Effect_Warp"), &vPos);

			m_iSuperArmor = 100;

			m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
			m_pGameInstance->Play_Sound(TEXT("SharinganSFx_BossAir"), SCH_EFFECT_MONSTER0);
			break;
		case Client::CKurama::State_Beaten:
			m_AnimationDesc.iAnimIndex = Anim_Beaten_Type01;
			m_AnimationDesc.bSkipInterpolation = true;
			for (size_t i = 0; i < 10; i++)
			{
				m_pFingerLights[i]->Off();
			}
			break;
		case Client::CKurama::State_Die:
			m_AnimationDesc.iAnimIndex = Anim_Dying_Type01;
			m_AnimationDesc.bSkipInterpolation = true;

			Safe_Release(m_pIndicator);
			m_pGameInstance->Attack_Player(nullptr, -100);
			m_pGameInstance->Delete_CollisionObject(this);

			m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
			m_pGameInstance->Play_Sound(TEXT("Kurama_Die2"), SCH_EFFECT_MONSTER0, 0.6f);
			break;
		case Client::CKurama::State_Bomb:
			break;
		case Client::CKurama::State_Blast:
			break;
		case Client::CKurama::State_MiniBlast:
			break;
		default:
			break;
		case Client::CKurama::State_None:
			break;
		}
		m_ePrevState = m_eState;
	}
}

void CKurama::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CKurama::State_Initiation:
		if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_etc_Appearance and m_pModelCom->Get_CurrentAnimPos() > 4.f and not m_hasPlayedSound)
		{
			m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
			m_pGameInstance->Play_Sound(TEXT("Boom_FireBall"), SCH_EFFECT_MONSTER0);
			m_hasPlayedSound = true;
		}
		if (m_pModelCom->IsAnimationFinished(Anim_etc_Appearance))
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_Ninjutsu_Roar;
			//m_pModelCom->Set_Animation(m_AnimationDesc);
			m_hasPlayedSound = false;
		}

		if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Ninjutsu_Roar and m_pModelCom->Get_CurrentAnimPos() > 34.f)
		{
			if (m_fTimer > 0.5f)
			{
				EffectInfo EffectInfo{};
				EffectInfo.vColor = _float4(0.175f, 0.175f, 0.35f, 1.f);
				EffectInfo.fScale = 15.f;
				XMStoreFloat4(&EffectInfo.vPos, XMVector4Transform(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LipMouthDownCenter")).r[3], m_pTransformCom->Get_World_Matrix()));
				EffectInfo.iType = 1;
				m_fTimer = {};
				m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &EffectInfo);
			}
			if (not m_hasPlayedSound)
			{
				m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
				m_pGameInstance->Play_Sound(TEXT("Roar"), SCH_EFFECT_MONSTER0);
				m_hasPlayedSound = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Ninjutsu_Roar))
		{
			m_hasInitiated = true;
			m_eState = State_Idle;
			m_fTimer = {};

			m_pGameInstance->PlayBGM(TEXT("Madara"), 0.5f);
			_bool isBoss = true;
			m_pIndicator = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Indicator"), &isBoss));
		}
		break;
	case Client::CKurama::State_Idle:
	{
		_float fRadian = fabs(acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos) - m_pTransformCom->Get_State(State::Pos), 0.f)), XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(State::Look), 0.f))))));

		if (fRadian > XMConvertToRadians(60.f))
		{
			m_eState = State_LookAt;
		}
	}
	break;
	case Client::CKurama::State_LookAt:
		_vector vOriginLook = XMLoadFloat3(&m_vOriginalLook);
		_vector vDirToPlayer = XMVectorSetY(XMVector3Normalize(m_pPlayerTransform->Get_State(State::Pos) - m_pTransformCom->Get_State(State::Pos)), 0.f);
		m_pTransformCom->LookAt_Dir(XMVectorLerp(vOriginLook, vDirToPlayer, m_fTimer));

		m_AnimationDesc = {};
		if (XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(State::Right), vDirToPlayer)) < 0)
		{
			m_AnimationDesc.iAnimIndex = Anim_Idle_TurnLeft180;
		}
		else
		{
			m_AnimationDesc.iAnimIndex = Anim_Idle_TurnRight180;
		}

		//m_pModelCom->Set_Animation(m_AnimationDesc);

		if (m_fTimer > 1.f)
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CKurama::State_Attack:
		break;
	case Client::CKurama::State_ComboAttack:
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_MowDown_Right))
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_MowDown_Left;
			//m_pModelCom->Set_Animation(m_AnimationDesc);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_MowDown_Left))
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_KickUp;
			//m_pModelCom->Set_Animation(m_AnimationDesc);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_KickUp))
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_DoubleSledgehammer;
			m_AnimationDesc.fDurationRatio = 1.f;
			//m_pModelCom->Set_Animation(m_AnimationDesc);
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_DoubleSledgehammer))
		{
			m_eState = State_Idle;
		}

		m_fTimer = {};
		break;
	case Client::CKurama::State_ComboAttack2:
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_SwingingDownScratch_Right))
		{
			m_AnimationDesc.iAnimIndex = Anim_Attack_UppercutScratch_Left;
			m_AnimationDesc.fDurationRatio = 1.f;
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Attack_UppercutScratch_Left))
		{
			m_eState = State_Idle;
		}

		m_fTimer = {};
		break;
	case Client::CKurama::State_MiniBomb:
		//m_pModelCom->Set_Animation(m_AnimationDesc);

		if (m_fTimer * 24.f > 22.f and m_fTimer * 24.f < 23.f or
			m_fTimer * 24.f > 32.f and m_fTimer * 24.f < 33.f or
			m_fTimer * 24.f > 42.f and m_fTimer * 24.f < 43.f)
		{
			if (not m_hasShot)
			{
				_vector vPlayerPosForLookAt = XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos), m_pTransformCom->Get_State(State::Pos).m128_f32[1]);
				m_pTransformCom->LookAt(vPlayerPosForLookAt);

				ObjectInfo Info{};
				XMStoreFloat4(&Info.vPos, XMVector4Transform(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("LipMouthDownCenter")).r[3], m_pTransformCom->Get_World_Matrix()));
				XMStoreFloat4(&Info.vLook, m_pPlayerTransform->Get_CenterPos());
				m_pGameInstance->Add_Layer(LEVEL_CLOUD, TEXT("Layer_MiniBomb"), TEXT("Prototype_GameObject_MiniBomb"), &Info);

				EffectInfo EffectInfo{};
				EffectInfo.vColor = _float4(0.175f, 0.175f, 0.35f, 1.f);
				EffectInfo.fScale = 15.f;
				EffectInfo.vPos = Info.vPos;
				EffectInfo.iType = 1;
				m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &EffectInfo);
				m_hasShot = true;

				m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
				m_pGameInstance->Play_Sound(TEXT("Hitted_ChidoriEnd"), SCH_EFFECT_MONSTER0);
			}
		}
		else
		{
			m_hasShot = false;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Ninjutsu_ConsecutiveTailedBeastBomb))
		{
			m_eState = State_Idle;
		}

		break;
	case Client::CKurama::State_Roar:
		break;
	case Client::CKurama::State_Warp:
		if (m_pModelCom->IsAnimationFinished(Anim_HandSeal_RecoveryChakra_Start))
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_HandSeal_RecoveryChakra_Loop;
			m_AnimationDesc.isLoop = true;
			m_fTimer = {};
			//m_pModelCom->Set_Animation(m_AnimationDesc);
		}

		if (m_fTimer > 4.f and m_pModelCom->Get_CurrentAnimationIndex() == Anim_HandSeal_RecoveryChakra_Loop)
		{
			for (size_t i = 0; i < 10; i++)
			{
				m_pFingerLights[i]->On();
			}

			EffectInfo FxInfo{};
			FxInfo.vColor = _float4(1.f, 1.f, 1.f, 1.f);
			FxInfo.fScale = 5.f;
			XMStoreFloat4(&FxInfo.vPos, m_pTransformCom->Get_State(State::Pos));
			FxInfo.vPos.y += 3.f;
			FxInfo.iType = 100;
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Smoke"), &FxInfo);

			m_pTransformCom->Set_Position(m_vAppearPoints[++m_iPosIndex]);

			m_pGameInstance->StopSound(SCH_EFFECT_MONSTER0);
			m_pGameInstance->Play_Sound(TEXT("Hitted_Chidori_0"), SCH_EFFECT_MONSTER0);

			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_HandSeal_RecoveryChakra_End;

			Safe_Release(m_pEffect);
		}

		if (m_pModelCom->IsAnimationFinished(Anim_HandSeal_RecoveryChakra_End))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CKurama::State_Beaten:

		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Type01))
		{
			m_eState = State_Idle;
			if (m_iPosIndex < 4)
			{
				m_eState = State_Warp;
			}
			m_hasTakenChidori = false;

			if (m_iHP <= 0)
			{
				m_eState = State_Die;
			}
		}

		break;
	case Client::CKurama::State_Die:
		if (m_pModelCom->IsAnimationFinished(Anim_Dying_Type01))
		{
			m_AnimationDesc.iAnimIndex = Anim_Dying_Type01_Loop;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.isLoop = true;
			CUI_Manager::Get_Instance()->Delete_HPBar(TEXT("Kurama"));
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Win"));
		}
		break;
	case Client::CKurama::State_Bomb:
		break;
	case Client::CKurama::State_Blast:
		break;
	case Client::CKurama::State_MiniBlast:
		break;
	case Client::CKurama::State_None:
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

	Safe_Release(m_pIndicator);
	Safe_Release(m_pEyeLights[0]);
	Safe_Release(m_pEyeLights[1]);
	for (size_t i = 0; i < 10; i++)
	{
		Safe_Release(m_pFingerLights[i]);
	}
	Safe_Release(m_pEffect);
	Safe_Release(m_pCollider_Hit);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayerTransform);
}
