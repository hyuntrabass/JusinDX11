#include "Sandman.h"

CSandman::CSandman(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSandman::CSandman(const CSandman& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSandman::Init_Prototype()
{
	return S_OK;
}

HRESULT CSandman::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (rand() & 2)
	{
		m_pModelCom->Set_Animation(Anim_etc_Appearance_Type01, false);
	}
	else
	{
		m_pModelCom->Set_Animation(Anim_etc_Appearance_Type02, false);
	}

	if (pArg)
	{
		m_vOriginPos = *reinterpret_cast<_float4*>(pArg);

		m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_vOriginPos));
	}

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);

	m_fWalkSpeed = 2.f;
	m_fRunSpeed = 5.f;
	m_fAttackRange = 2.f;
	m_fSearchRange = 20.f;

	return S_OK;
}

void CSandman::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	//{
	//	return;
	//}

	//if (m_pModelCom->IsAnimationFinished(m_pModelCom->Get_CurrentAnimationIndex()))
	//{
	//	m_pModelCom->Set_Animation(rand() % 144, false);
	//	Move(fTimeDelta);
	//}

	if (m_pModelCom->IsAnimationFinished(Anim_etc_Appearance_Type01) or m_pModelCom->IsAnimationFinished(Anim_etc_Appearance_Type02))
	{
		m_pModelCom->Set_Animation(Anim_Idle_Type02_Loop, true);
	}

	Artificial_Intelligence(fTimeDelta);

	Change_State();

	Idle(fTimeDelta);
	Move(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CSandman::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CSandman::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_float fNorTex = 0.f;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			fNorTex = 0.f;
		}
		else
		{
			fNorTex = 1.f;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNorTex", &fNorTex, sizeof _float)))
		{
			return E_FAIL;
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
	return S_OK;
}

HRESULT CSandman::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sandman"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSandman::Bind_ShaderResources()
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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_CREATECHARACTER, 0);
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

void CSandman::Artificial_Intelligence(_float fTimeDelta)
{
	if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Attack_SideDoubleSlashing and
		not m_pModelCom->IsAnimationFinished(Anim_Attack_SideDoubleSlashing))
	{
		return;
	}
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vector vMyPos = m_pTransformCom->Get_State(State::Pos);
	m_vOriginPos.y = vMyPos.m128_f32[1];
	_vector vDirToPlayer = vPlayerPos - vMyPos;
	_float fDist = XMVectorGetX(XMVector3Length(vDirToPlayer));
	_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

	if (fDist < m_fAttackRange)
	{
		XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
		m_vTargetDir.y = 0.f;

		m_eCurrState = State_Attack;
	}
	else if (fDist > m_fSearchRange)
	{
		fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vOriginPos) - vMyPos));
		if (fDist > 1.f)
		{
			XMStoreFloat4(&m_vTargetDir, XMLoadFloat4(&m_vOriginPos) - vMyPos);
			m_vTargetDir.y = 0.f;
			m_eCurrState = State_Walk;
		}
		else
		{
			m_eCurrState = State_Idle;

			if (iCurrentAnimIndex == Anim_Run_Loop)
			{
				m_pModelCom->Set_Animation(Anim_Run_End, false);
			}
			else if (iCurrentAnimIndex == Anim_Walk_Loop)
			{
				m_pModelCom->Set_Animation(Anim_Walk_End, false);
			}

			if (m_pModelCom->IsAnimationFinished(Anim_Run_End) or m_pModelCom->IsAnimationFinished(Anim_Walk_End))
			{
				m_pModelCom->Set_Animation(Anim_Idle_Type02_Loop, true);
			}
		}
	}
	else
	{
		XMStoreFloat4(&m_vTargetDir, vDirToPlayer);
		m_vTargetDir.y = 0.f;
		m_eCurrState = State_Run;
	}

	m_fAttTime += fTimeDelta;

	if (m_eCurrState == State_Attack)
	{
		m_pTransformCom->Look_At_Dir(XMLoadFloat4(&m_vTargetDir));

		if (m_fAttTime > 2.f)
		{
			m_pModelCom->Set_Animation(Anim_Attack_SideDoubleSlashing, false);
			m_fAttTime = 0.f;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Attack_SideDoubleSlashing))
		{
			m_pModelCom->Set_Animation(Anim_Idle_Type02_Loop, true);
		}
	}
}

void CSandman::Idle(_float fTimeDelta)
{
}

void CSandman::Move(_float fTimeDelta)
{
	if (m_eCurrState == State_Run)
	{
		m_pTransformCom->Look_At_Dir(XMLoadFloat4(&m_vTargetDir));
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (m_eCurrState == State_Walk)
	{
		m_pTransformCom->Look_At_Dir(XMLoadFloat4(&m_vTargetDir));
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
}

void CSandman::Change_State()
{
	if (m_eCurrState not_eq m_ePrevState)
	{
		switch (m_eCurrState)
		{
		case Client::CSandman::State_Idle:
			break;
		case Client::CSandman::State_Run:
			m_pTransformCom->Set_Speed(m_fRunSpeed);
			m_pModelCom->Set_Animation(Anim_Run_Loop, true);
			break;
		case Client::CSandman::State_Walk:
			m_pTransformCom->Set_Speed(m_fWalkSpeed);
			m_pModelCom->Set_Animation(Anim_Walk_Loop, true);
			break;
		case Client::CSandman::State_Attack:
			break;
		case Client::CSandman::State_Die:
			break;
		}

		m_ePrevState = m_eCurrState;
	}
}

CSandman* CSandman::Create(_dev pDevice, _context pContext)
{
	CSandman* pInstance = new CSandman(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSandman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSandman::Clone(void* pArg)
{
	CSandman* pInstance = new CSandman(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSandman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSandman::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
