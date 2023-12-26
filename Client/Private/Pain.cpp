#include "Pain.h"
#include "UI_Manager.h"

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

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

	m_AnimationDesc.iAnimIndex = Anim_Idle_Loop001;
	m_AnimationDesc.isLoop = true;
	m_eState = State_Idle;

	m_iHP = 300;

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
	m_fTimer += fTimeDelta;
}

void CPain::Late_Tick(_float fTimeDelta)
{
	m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_eState != State_None)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

	#ifdef _DEBUG
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
	if (m_eState == State_Beaten_Electric and iDamageType == DAM_ELECTRIC)
	{
		return;
	}
	m_iHP -= iDamage;

	if (iDamageType == DAM_ELECTRIC)
	{
		m_eState = State_Beaten_Electric;
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
			break;
		case Client::CPain::State_Idle:
			m_AnimationDesc.iAnimIndex = Anim_Idle_Loop001;
			m_AnimationDesc.isLoop = true;
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
		case Client::CPain::State_Die:
			break;
		default:
			break;
		}

		m_ePrevState = m_eState;
	}
}

void CPain::Tick_State(_float fTimeDelta)
{
	//m_AnimationDesc = {};
	switch (m_eState)
	{
	case Client::CPain::State_None:
		break;
	case Client::CPain::State_Initiation:
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
				m_fTimer = {};
				m_eState = State_Idle;
			}
		}
		break;
	case Client::CPain::State_Pull:
		break;
	case Client::CPain::State_Sommon:
		break;
	case Client::CPain::State_Beaten:
		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_Right))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CPain::State_Beaten_Electric:
		if (m_fTimer > 1.5f)
		{
			m_AnimationDesc = {};
			m_AnimationDesc.iAnimIndex = Anim_Beaten_ElectricShock_End;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_Beaten_ElectricShock_End))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CPain::State_Die:
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

	Safe_Release(m_pCollider_Hit);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayerTransform);
}
