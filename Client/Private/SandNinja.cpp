#include "SandNinja.h"
#include "UI_Manager.h"
#include "Indicator.h"
#include "Trigger_Manager.h"

CSandNinja::CSandNinja(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSandNinja::CSandNinja(const CSandNinja& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSandNinja::Init_Prototype()
{
	return S_OK;
}

HRESULT CSandNinja::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);

	m_eCurrState = State_Appear;

	if (pArg)
	{
		ObjectInfo Info = *reinterpret_cast<ObjectInfo*>(pArg);

		m_vOriginPos = Info.vPos;

		m_pTransformCom->Set_Position(_float3(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z));
		m_pTransformCom->LookAt_Dir(XMLoadFloat4(&Info.vLook));
	}

	m_fWalkSpeed = 2.f;
	m_fRunSpeed = 5.f;
	m_fAttackRange = 40.f;
	m_fSearchRange = 50.f;

	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);
	m_iHP = 20;

	EffectInfo FxInfo{};
	FxInfo.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	FxInfo.fScale = 2.f;
	FxInfo.vPos = _float4(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z, 1.f);
	FxInfo.iType = 70;
	m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Smoke"), &FxInfo);

	_bool isBoss = false;
	m_pIndicator = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Indicator"), &isBoss));
	if (not m_pIndicator)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSandNinja::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	//{
	//	return;
	//}

#ifdef _DEBUGTEST
	if (m_pGameInstance->Key_Down(DIK_U))
	{
		_float3 vNewPos{};
		XMStoreFloat3(&vNewPos, m_pTransformCom->Get_State(State::Pos) + XMVectorSet(0.f, 10.f, 0.f, 0.f));
		m_pTransformCom->Set_Position(vNewPos);
	}
#endif // DEBUG

	//if (m_pTransformCom->Get_State(State::Pos).m128_f32[1] < 20.f)
	//{
	//	m_iHP = 0;
	//	ANIM_DESC Anim{};
	//	Anim.iAnimIndex = Anim_Dying_Aerial_Fall_Behind_Loop;
	//	Anim.isLoop = true;
	//	m_pModelCom->Set_Animation(Anim);
	//	m_pTransformCom->Gravity(fTimeDelta);
	//	if (m_pModelCom->IsAnimationFinished(Anim_Dying_Aerial_Fall_Behind_Loop))
	//	{
	//		m_isDead = true;
	//	}
	//	return;
	//}


	if (not CTrigger_Manager::Get_Instance()->Hasto_PlayScene())
	{
		Artificial_Intelligence(fTimeDelta);
	}

	m_pTransformCom->Gravity(fTimeDelta);

	m_pCollider_Hit->Update(m_pTransformCom->Get_World_Matrix());

	if (m_pIndicator)
	{
		_float fCamDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()) - m_pTransformCom->Get_State(State::Pos)));

		_float3 v2DPos{};
		XMStoreFloat3(&v2DPos, XMVector3Project(m_pTransformCom->Get_State(State::Pos), 0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, m_pGameInstance->Get_Transform(TransformType::Proj), m_pGameInstance->Get_Transform(TransformType::View), XMMatrixTranslation(0.f, Lerp(1.5f, 2.3f, fCamDist / 50.f), 0.f)));

		if (v2DPos.z > 1.f)
		{
			v2DPos = _float3(-1.f, -1.f, -1.f);
		}


		m_pIndicator->Tick(_float2(v2DPos.x, v2DPos.y));
	}
}

void CSandNinja::Late_Tick(_float fTimeDelta)
{
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pIndicator)
	{
		m_pIndicator->Late_Tick(fTimeDelta);
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
	{

		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

	#ifdef _DEBUGTEST
		m_pRendererCom->Add_DebugComponent(m_pCollider_Hit);
	#endif // _DEBUG
	}
	else
	{
		int a = 10;
	}
}

HRESULT CSandNinja::Render()
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

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
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

		_uint iPassIndex{ AnimPass_Default };

		if (m_eCurrState == State_Die)
		{
			iPassIndex = AnimPass_Dissolve;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof m_fDissolveRatio)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
			{
				return E_FAIL;
			}
		}

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
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

void CSandNinja::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (m_iHP <= 0 or m_eCurrState == State_Die or m_eCurrState == State_Beaten_Electiric and iDamageType == DAM_ELECTRIC)
	{
		return;
	}
	m_iHP -= iDamage;

	m_ePrevState = State_None;
	if (iDamageType == DAM_ELECTRIC)
	{
		m_eCurrState = State_Beaten_Electiric;
	}
	else if (m_iHP <= 0)
	{
		m_eCurrState = State_Die;
	}
	else if (iDamageType == DAM_FIRE)
	{
		m_eCurrState = State_Beaten_Fire;
	}
	else
	{
		m_eCurrState = State_Beaten;
	}

	CUI_Manager::Get_Instance()->Create_Hit();
}

HRESULT CSandNinja::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_SandNinja"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_08"), TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::AABB;
	ColDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
	ColDesc.vCenter = _float3(0.f, ColDesc.vExtents.y, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pCollider_Hit), &ColDesc)))
	{
		return E_FAIL;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

HRESULT CSandNinja::Bind_ShaderResources()
{
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_Hit, sizeof _bool)))
	//{
	//	return E_FAIL;
	//}

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

void CSandNinja::Artificial_Intelligence(_float fTimeDelta)
{
	Init_State();
	Tick_State(fTimeDelta);

	m_fTimer += fTimeDelta;
}

void CSandNinja::Init_State()
{
	if (m_eCurrState != m_ePrevState)
	{
		ANIM_DESC Anim{};
		switch (m_eCurrState)
		{
		case Client::CSandNinja::State_Appear:
			if (rand() & 2)
			{
				Anim.iAnimIndex = Anim_etc_Appearance_Type01;
			}
			else
			{
				Anim.iAnimIndex = Anim_etc_Appearance_Type02;
			}

			break;
		case Client::CSandNinja::State_Idle:
			Anim.iAnimIndex = Anim_Idle_Type02_Loop;
			Anim.isLoop = true;
			break;
		case Client::CSandNinja::State_Chase:
			m_pTransformCom->Set_Speed(m_fRunSpeed);
			Anim.iAnimIndex = Anim_Run_Loop;
			Anim.isLoop = true;
			break;
		case Client::CSandNinja::State_Runaway:
			m_pTransformCom->Set_Speed(m_fRunSpeed);
			Anim.iAnimIndex = Anim_Run_Loop;
			Anim.isLoop = true;
			break;
		case Client::CSandNinja::State_GoHome:
			m_pTransformCom->Set_Speed(m_fWalkSpeed);
			Anim.iAnimIndex = Anim_Walk_Loop;
			Anim.isLoop = true;
			break;
		case Client::CSandNinja::State_Beaten:
			if (rand() % 2)
			{
				Anim.iAnimIndex = Anim_Beaten_Left;
			}
			else
			{
				Anim.iAnimIndex = Anim_Beaten_Right;
			}
			Anim.bSkipInterpolation = true;
			Anim.bRestartAnimation = true;

			break;
		case Client::CSandNinja::State_Beaten_Electiric:
		{
			Anim.iAnimIndex = Anim_Beaten_ElectricShock_Loop;
			Anim.isLoop = true;
			Anim.bSkipInterpolation = true;
			Anim.bRestartAnimation = true;

			_float3 vPos{};
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_LeftCidori"), TEXT("Prototype_GameObject_LeftChidori"), &vPos);

			m_fTimer = {};
			break;
		}
		case Client::CSandNinja::State_Beaten_Fire:
			Anim.iAnimIndex = Anim_Beaten_Burn_Type01;
			Anim.bSkipInterpolation = true;
			Anim.bRestartAnimation = true;
			break;
		case Client::CSandNinja::State_Attack:
			Anim.iAnimIndex = Anim_Throw_Kunai;
			m_fTimer = {};
			m_bAttacked = false;
			break;
		case Client::CSandNinja::State_Die:
			Anim.iAnimIndex = Anim_Dying_Type01;
			Anim.bSkipInterpolation = true;

			Safe_Release(m_pIndicator);
			m_pGameInstance->Attack_Player(nullptr, -5);
			m_pGameInstance->Delete_CollisionObject(this);
			_float4 vPos{};
			XMStoreFloat4(&vPos, m_pTransformCom->Get_CenterPos());

			m_fTimer = {};
			break;
		}

		m_pModelCom->Set_Animation(Anim);
		m_ePrevState = m_eCurrState;
	}
}

void CSandNinja::Tick_State(_float fTimeDelta)
{
	switch (m_eCurrState)
	{
	case Client::CSandNinja::State_Appear:
		if (m_pModelCom->IsAnimationFinished(Anim_etc_Appearance_Type01) or m_pModelCom->IsAnimationFinished(Anim_etc_Appearance_Type02))
		{
			m_eCurrState = State_Idle;
		}
		break;
	case Client::CSandNinja::State_Idle:
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
		m_vOriginPos.y = vMyPos.m128_f32[1];
		_vector vDirToPlayer = vPlayerPos - vMyPos;
		_float fPlayerDist = XMVectorGetX(XMVector3Length(vDirToPlayer));
		_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

		if (fPlayerDist < m_fAttackRange)
		{
			if (m_fTimer > 4.f)
			{
				XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
				m_vTargetDir.y = 0.f;

				m_eCurrState = State_Attack;
			}
		}
		else if (fPlayerDist < m_fSearchRange)
		{
			m_eCurrState = State_Chase;
		}
		if (fPlayerDist < 7.f)
		{
			m_eCurrState = State_Runaway;
		}
		break;
	}
	case Client::CSandNinja::State_Chase:
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
		m_vOriginPos.y = vMyPos.m128_f32[1];
		_vector vDirToPlayer = vPlayerPos - vMyPos;
		_float fTargetDist = XMVectorGetX(XMVector3Length(vDirToPlayer));

		XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
		m_vTargetDir.y = 0.f;

		if (fTargetDist > m_fSearchRange)
		{
			ANIM_DESC Anim{};
			Anim.iAnimIndex = Anim_Run_End;
			m_pModelCom->Set_Animation(Anim);

		}
		else if (fTargetDist < m_fAttackRange)
		{
			if (m_fTimer > 4.f)
			{
				XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
				m_vTargetDir.y = 0.f;

				m_eCurrState = State_Attack;
			}
		}
		else
		{
			m_pTransformCom->LookAt_Dir(XMLoadFloat4(&m_vTargetDir));
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Run_End))
		{
			m_eCurrState = State_GoHome;
		}
		break;
	}
	case Client::CSandNinja::State_Runaway:
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
		m_vOriginPos.y = vMyPos.m128_f32[1];
		_vector vDirToPlayer = vPlayerPos - vMyPos;
		_float fTargetDist = XMVectorGetX(XMVector3Length(vDirToPlayer));

		XMStoreFloat4(&m_vTargetDir, vDirToPlayer * -1.f);
		m_vTargetDir.y = 0.f;

		if (fTargetDist > 10.f)
		{
			ANIM_DESC Anim{};
			Anim.iAnimIndex = Anim_Run_End;
			m_pModelCom->Set_Animation(Anim);

		}
		else
		{
			m_pTransformCom->LookAt_Dir(XMLoadFloat4(&m_vTargetDir));
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Run_End))
		{
			m_eCurrState = State_Idle;
		}
		break;
	}
	case Client::CSandNinja::State_GoHome:
	{
		m_pTransformCom->LookAt(XMVectorSetY(XMLoadFloat4(&m_vOriginPos), m_pTransformCom->Get_State(State::Pos).m128_f32[1]));
		m_pTransformCom->Go_Straight(fTimeDelta);

		_float fHomeDist = XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat4(&m_vOriginPos), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(State::Pos), 0.f)));

		if (fHomeDist < 1.f)
		{
			ANIM_DESC Anim{};
			Anim.iAnimIndex = Anim_Walk_End;
			m_pModelCom->Set_Animation(Anim);
		}

		_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
		m_vOriginPos.y = vMyPos.m128_f32[1];
		_vector vDirToPlayer = vPlayerPos - vMyPos;
		_float fPlayerDist = XMVectorGetX(XMVector3Length(vDirToPlayer));
		_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

		if (fPlayerDist < m_fAttackRange)
		{
			if (m_fTimer > 4.f)
			{
				XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
				m_vTargetDir.y = 0.f;

				m_eCurrState = State_Attack;
			}
		}
		else if (fPlayerDist < m_fSearchRange)
		{
			m_eCurrState = State_Chase;
		}
		if (fPlayerDist < 7.f)
		{
			m_eCurrState = State_Runaway;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Walk_End))
		{
			m_eCurrState = State_Idle;
		}
		break;
	}
	case Client::CSandNinja::State_Beaten:
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Left))
		{
			m_eCurrState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eCurrState = State_Die;
			}
		}
		break;
	case Client::CSandNinja::State_Beaten_Electiric:
		if (m_fTimer > 1.f)
		{
			if (m_iHP > 0)
			{
				ANIM_DESC Anim{};
				Anim.iAnimIndex = Anim_Beaten_ElectricShock_End;
				m_pModelCom->Set_Animation(Anim);
			}
			else
			{
				m_eCurrState = State_Die;
			}
		}
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_ElectricShock_End))
		{
			m_eCurrState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eCurrState = State_Die;
			}
		}
		break;
	case Client::CSandNinja::State_Beaten_Fire:
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Burn_Type01))
		{
			m_eCurrState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eCurrState = State_Die;
			}
		}
		break;
	case Client::CSandNinja::State_Attack:
		m_pTransformCom->LookAt_Dir(XMLoadFloat4(&m_vTargetDir));

		if (not m_bAttacked and m_fTimer > 0.9f)
		{
			_float4 vPos{};
			XMStoreFloat4(&vPos, XMVector4Transform(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("R_Hand_Weapon_cnt_tr")).r[3], m_pTransformCom->Get_World_Matrix()));
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Kunai"), TEXT("Prototype_GameObject_SandKunai"), &vPos);
			m_bAttacked = true;
			m_fTimer = {};
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Throw_Kunai))
		{
			m_eCurrState = State_Idle;
		}

		break;
	case Client::CSandNinja::State_Die:
		if (m_fTimer > 10.f)
		{
			m_fDissolveRatio += fTimeDelta / 5.f;
		}
		if (m_fDissolveRatio > 1.f)
		{
			m_isDead = true;
		}

		break;
	default:
		break;
	}
}

CSandNinja* CSandNinja::Create(_dev pDevice, _context pContext)
{
	CSandNinja* pInstance = new CSandNinja(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSandNinja");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSandNinja::Clone(void* pArg)
{
	CSandNinja* pInstance = new CSandNinja(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSandNinja");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSandNinja::Free()
{
	__super::Free();

	Safe_Release(m_pIndicator);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCollider_Hit);
}
