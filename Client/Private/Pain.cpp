#include "Pain.h"
#include "UI_Manager.h"
#include "Indicator.h"
#include "Trigger_Manager.h"

CPain::CPain(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPain::CPain(const CPain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPain::Init_Prototype()
{
	return S_OK;
}

HRESULT CPain::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, XMVectorSet(100.f, 45.f, -75.f, 1.f));
	m_pTransformCom->LookAt_Dir(XMVectorSet(-1.f, 0.f, 1.f, 0.f));

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

	m_eState = State_Initiation;

	m_iMaxHP = 300;
	m_iHP = m_iMaxHP;

	_bool isBoss = true;
	m_pIndicator = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Indicator"), &isBoss));
	if (not m_pIndicator)
	{
		return E_FAIL;
	}

	_uint iHPBarType{ 1 };
	m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI_HpBar"), TEXT("Prototype_GameObject_UI_BossHpBar"), &iHPBarType);
	m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI_HpBar_Base"), TEXT("Prototype_GameObject_UI_BossHpBar_Base"), &iHPBarType);

	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("Pain"), 0.5f);

	return S_OK;
}

void CPain::Tick(_float fTimeDelta)
{
	Artificial_Intelligence(fTimeDelta);

	//if (m_pModelCom->IsAnimationFinished(m_AnimationDesc.iAnimIndex))
	//{
	//    m_AnimationDesc.iAnimIndex++;
	//    if (m_AnimationDesc.iAnimIndex >= Anim_End)
	//    {
	//        m_AnimationDesc.iAnimIndex = 0;
	//    }
	m_pModelCom->Set_Animation(m_AnimationDesc);
	m_AnimationDesc.bRestartAnimation = false;
	//}


	m_pTransformCom->Gravity(fTimeDelta);

	_matrix ColliderOffset = XMMatrixTranslation(0.f, 0.8f, 0.f);
	m_pCollider_Hit->Update(/*ColliderOffset * */m_pTransformCom->Get_World_Matrix());

	if (m_pIndicator)
	{
		_float fCamDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()) - m_pTransformCom->Get_State(State::Pos)));

		_float3 v2DPos{};
		XMStoreFloat3(&v2DPos, XMVector3Project(m_pTransformCom->Get_State(State::Pos), 0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, m_pGameInstance->Get_Transform(TransformType::Proj), m_pGameInstance->Get_Transform(TransformType::View), XMMatrixTranslation(0.f, Lerp(1.8f, 4.0f, fCamDist / 50.f), 0.f)));

		if (v2DPos.z > 1.f)
		{
			v2DPos = _float3(-1.f, -1.f, -1.f);
		}

		m_pIndicator->Tick(_float2(v2DPos.x, v2DPos.y));
	}

	if (m_eState != State_Die)
	{
		CUI_Manager::Get_Instance()->Set_HP(TEXT("Pain"), m_iMaxHP, m_iHP);
	}
}

void CPain::Late_Tick(_float fTimeDelta)
{
	if (not m_hasInitiated)
	{
		return;
	}
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pIndicator)
	{
		m_pIndicator->Late_Tick(fTimeDelta);
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_eState != State_None)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

	#ifdef _DEBUGG
		m_pRendererCom->Add_DebugComponent(m_pCollider_Hit);
	#endif // _DEBUG
	}
}

HRESULT CPain::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_bool hasNormal{};
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{

		}

		if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			hasNormal = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &hasNormal, sizeof _bool)))
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

void CPain::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (m_eState == State_Die)
	{
		return;
	}
	if (m_eState == State_Beaten_Electric and iDamageType == DAM_ELECTRIC)
	{
		return;
	}
	m_iHP -= iDamage;
	if (m_iHP < 0)
	{
		m_iHP = 0;
		m_eState = State_Die;
		CUI_Manager::Get_Instance()->Create_Hit();
		if (m_ePrevState == State_Push)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
		return;
	}

	if (iDamage > m_iSuperArmor)
	{
		if (iDamageType == DAM_ELECTRIC)
		{
			m_eState = State_Beaten_Electric;
		}
		else if (iDamageType == DAM_FIRE)
		{
			m_eState = State_Beaten_Fire;
		}
		else
		{
			m_eState = State_Beaten;
		}

		if (m_ePrevState == State_Push)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
		m_ePrevState = State_None;
	}
	else
	{
		CUI_Manager::Get_Instance()->Create_Hit();
	}
}

HRESULT CPain::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Model_Pain"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::AABB;
	ColDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
	ColDesc.vCenter = _float3(0.f, ColDesc.vExtents.y, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pCollider_Hit), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPain::Bind_ShaderResources()
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

void CPain::Artificial_Intelligence(_float fTimeDelta)
{
	Init_State();
	Tick_State(fTimeDelta);
}

void CPain::Init_State()
{
	if (m_eState != m_ePrevState)
	{
		if (m_ePrevState == State_Push)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
		m_AnimationDesc = {};
		switch (m_eState)
		{
		case Client::CPain::State_None:
			break;
		case Client::CPain::State_Initiation:
			m_fTimer = {};
			break;
		case Client::CPain::State_Idle:
			m_AnimationDesc.iAnimIndex = Anim_Idle_Loop001;
			m_AnimationDesc.isLoop = true;

			m_iSuperArmor = 0;
			break;
		case Client::CPain::State_LookAt:
			break;
		case Client::CPain::State_ComboAttack:
			break;
		case Client::CPain::State_Push:
			m_AnimationDesc.iAnimIndex = Anim_Ninjutsu_AlmightyPush_Start;
			m_pTransformCom->LookAt_Dir(XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos) - m_pTransformCom->Get_State(State::Pos), 0.f));
			m_fTimer = {};
			m_pGameInstance->Set_TimeRatio(0.3f);

			m_iSuperArmor = 50;
			break;
		case Client::CPain::State_Pull:
			break;
		case Client::CPain::State_Sommon:
			break;
		case Client::CPain::State_Beaten:
			m_AnimationDesc.iAnimIndex = Anim_Beaten_Right;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.bRestartAnimation = true;
			CUI_Manager::Get_Instance()->Create_Hit();
			break;
		case Client::CPain::State_Beaten_Electric:
			m_AnimationDesc.iAnimIndex = Anim_Beaten_ElectricShock_Loop;
			m_AnimationDesc.isLoop = true;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.bRestartAnimation = true;
			CUI_Manager::Get_Instance()->Create_Hit();

			m_fTimer = {};
			break;
		case Client::CPain::State_Beaten_Fire:
			m_AnimationDesc.iAnimIndex = Anim_Beaten_Burn_Type01;
			m_AnimationDesc.bSkipInterpolation = true;
			m_AnimationDesc.bRestartAnimation = true;
			CUI_Manager::Get_Instance()->Create_Hit();
			break;
		case Client::CPain::State_Die:
			m_AnimationDesc.iAnimIndex = Anim_Dying_Type01;
			m_AnimationDesc.bSkipInterpolation = true;

			Safe_Release(m_pIndicator);
			m_pGameInstance->Attack_Player(nullptr, -100);
			m_pGameInstance->Delete_CollisionObject(this);
			m_fTimer = {};
			break;
		default:
			break;
		}

		m_ePrevState = m_eState;
	}
}
// 고통을 받아들여라.... 고통을 알아라...
// 고통을 모르는자는 진정한 평화를 알수없어!
// 4.803628
// 6.824368
// 13.124854
// 15.376422
// 18.760143
void CPain::Tick_State(_float fTimeDelta)
{
	//m_AnimationDesc = {};
	switch (m_eState)
	{
	case Client::CPain::State_None:
		break;
	case Client::CPain::State_Initiation:
		if (not m_hasInitiated and m_fTimer > 1.f)
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_etc_Appearance;
			m_hasInitiated = true;
		}
		if (m_pModelCom->IsAnimationFinished(Anim_etc_Appearance))
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_Idle_Loop001;
			m_AnimationDesc.isLoop = true;

			m_pGameInstance->StopSound(SCH_NPC2);
			m_pGameInstance->Play_Sound(TEXT("Pain_Voice"), SCH_NPC2);
		}
		if (m_iLineNumber == 0 and m_fTimer > 4.f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT("고통을 느껴라...."));
			m_iLineNumber++;
		}
		if (m_iLineNumber == 1 and m_fTimer > 6.f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT("고통을 생각해라...."));
			m_iLineNumber++;
		}
		if (m_iLineNumber == 2 and m_fTimer > 8.f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT(""));
			m_iLineNumber++;
		}
		if (m_iLineNumber == 3 and m_fTimer > 12.6f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT("고통을 받아들여라...."));
			m_iLineNumber++;
		}
		if (m_iLineNumber == 4 and m_fTimer > 14.7f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT("고통을 알아라..."));
			m_iLineNumber++;
		}
		if (m_iLineNumber == 5 and m_fTimer > 18.f)
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT("고통을 모르는자는 진정한 평화를 알수없어!!!"));
			m_iLineNumber++;
		}

		m_fTimer += fTimeDelta;

		if (not CTrigger_Manager::Get_Instance()->Hasto_PlayScene())
		{
			CUI_Manager::Get_Instance()->Set_SubTitle(TEXT(""));
			m_eState = State_Idle;
		}
		break;
	case Client::CPain::State_Idle:
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
		_float fDistToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

		if (fDistToPlayer < 10.f)
		{
			m_eState = State_Push;
		}

		break;
	}
	case Client::CPain::State_LookAt:
		break;
	case Client::CPain::State_ComboAttack:
		break;
	case Client::CPain::State_Push:
		if (m_fTimer > 0.75f)
		{
			if (not m_hasPushed)
			{
				m_pPlayerTransform->Jump(20.f);
				m_pGameInstance->Set_TimeRatio(1.f);
				m_pGameInstance->Attack_Player(nullptr, 0, DAM_PUSH);
				m_hasPushed = true;
			}
			m_pPlayerTransform->Set_Speed(30.f);
			m_pPlayerTransform->Go_Backward(fTimeDelta);

			_vector vPlayer2DPos = XMVectorSetY(m_pPlayerTransform->Get_State(State::Pos), 0.f);
			_vector vMy2DPos = XMVectorSetY(m_pTransformCom->Get_State(State::Pos), 0.f);
			_float fXZDistToPlayer = XMVectorGetX(XMVector3Length(vPlayer2DPos - vMy2DPos));

			if (m_pModelCom->IsAnimationFinished(Anim_Ninjutsu_AlmightyPush_Start))
				//if (fXZDistToPlayer > 30.f)
			{
				m_hasPushed = false;
				m_eState = State_Idle;
			}
		}

		m_fTimer += fTimeDelta;
		
		break;
	case Client::CPain::State_Pull:
		break;
	case Client::CPain::State_Sommon:
		break;
	case Client::CPain::State_Beaten:
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Right))
		{
			m_eState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eState = State_Die;
			}
		}
		break;
	case Client::CPain::State_Beaten_Electric:
		if (m_fTimer > 1.5f)
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_Beaten_ElectricShock_End;
		}
		
		m_fTimer += fTimeDelta;

		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_ElectricShock_End))
		{
			m_eState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eState = State_Die;
			}
		}
		break;
	case Client::CPain::State_Beaten_Fire:
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Burn_Type01))
		{
			m_eState = State_Idle;

			if (m_iHP <= 0)
			{
				m_eState = State_Die;
			}
		}
		break;
	case Client::CPain::State_Die:
		if (m_pModelCom->IsAnimationFinished(Anim_Dying_Type01))
		{
			CUI_Manager::Get_Instance()->Delete_HPBar(TEXT("Pain"));
		}

		if (m_fTimer > 5.f and m_fTimer < 10.f)
		{
			m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Win"));
			m_fTimer = 20.f;
		}
		else
		{
			m_fTimer += fTimeDelta;
		}
		break;
	default:
		break;
	}
}

CPain* CPain::Create(_dev pDevice, _context pContext)
{
	CPain* pInstance = new CPain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPain::Clone(void* pArg)
{
	CPain* pInstance = new CPain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPain::Free()
{
	__super::Free();

	Safe_Release(m_pIndicator);
	Safe_Release(m_pCollider_Hit);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayerTransform);
}
