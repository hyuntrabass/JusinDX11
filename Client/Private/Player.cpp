#include "Player.h"
#include "PhysX_Manager.h"
#include "BodyPart.h"
#include "UI_Manager.h"

CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Ready_Parts()))
	{
		MSG_BOX("Failed to Add Parts");
		return E_FAIL;
	}
	m_Animation = {};
	m_Animation.iAnimIndex = etc_Appearance;
	m_Animation.bSkipInterpolation = true;

	m_pTransformCom->Set_Speed(5.f);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit, true);
	m_iHP = 300;

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	{
		return;
	}

	m_fAttTimer += fTimeDelta;

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CREATECHARACTER)
	{
		if (not m_isGameStarted)
		{
			m_eState = Player_State::Idle;

			m_isGameStarted = true;
		}
		Move(fTimeDelta);
		Apply_State(fTimeDelta);

		//m_pGameInstance->Update_PhysX(m_pTransformCom);
	}
	else
	{
		Customize(fTimeDelta);
	}

	//if (m_pGameInstance->Key_Down(DIK_L))
	//{
	//	m_pTransformCom->WallTest();
	//}

	for (size_t i = 0; i < PT_END; i++)
	{
		m_pBodyParts[i]->Tick(fTimeDelta);
	}

	_matrix ColliderOffset = XMMatrixTranslation(0.f, 0.8f, 0.f);
	m_pCollider_Att->Update(ColliderOffset * m_pTransformCom->Get_World_Matrix());
	m_pCollider_Hit->Update(m_pTransformCom->Get_World_Matrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Get_CameraModeIndex() == CM_MAIN)
	{
		GET_CURSOR_POINT(pt);
		_vector Pos = m_pTransformCom->Get_State(State::Pos);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD());
		cout << "Cursor X : " << pt.x << endl;
		cout << "Cursor Y : " << pt.y << endl;
		cout << endl;
		cout << "PlayerPos X :" << Pos.m128_f32[0] << endl;
		cout << "PlayerPos Y :" << Pos.m128_f32[1] << endl;
		cout << "PlayerPos Z :" << Pos.m128_f32[2] << endl;
		cout << endl;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
#endif // _DEBUG

	for (size_t i = 0; i < PT_END; i++)
	{
		m_pBodyParts[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CPlayer::Render()
{
	for (size_t i = 0; i < PT_END; i++)
	{
		if (FAILED(m_pBodyParts[i]->Render()))
		{
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	m_pCollider_Att->Render();
	m_pCollider_Hit->Render();
#endif // _DEBUG


	return S_OK;
}

void CPlayer::Move(_float fTimeDelta)
{
	_bool hasMoved{};
	_vector vCamLook = XMLoadFloat4(&m_pGameInstance->Get_CameraLook());
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook);
	_vector vDirection{};

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		//m_pTransformCom->Go_Straight(fTimeDelta);
		vDirection += vCamLook;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		vDirection -= vCamLook;
		hasMoved = true;
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		vDirection += vCamRight;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		vDirection -= vCamRight;
		hasMoved = true;
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_eState != Player_State::DoubleJump)
	{
		if (m_eState == Player_State::Jump or
			m_eState == Player_State::Jump_Front)
		{
			m_eState = Player_State::DoubleJump;
		}
		else if (hasMoved)
		{
			m_eState = Player_State::Jump_Front;
		}
		else
		{
			m_eState = Player_State::Jump;
		}
		m_pTransformCom->Jump(15.f);
	}

	if (hasMoved)
	{
		//if (not m_pTransformCom->Is_Jumping())
		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if (m_eState != Player_State::Jump and
				m_eState != Player_State::Jump_Front and
				m_eState != Player_State::DoubleJump)
			{
				m_eState = Player_State::Run;
			}
			m_pTransformCom->Set_Speed(15.f);
		}
		else
		{
			if (m_eState != Player_State::Jump and
				m_eState != Player_State::Jump_Front and
				m_eState != Player_State::DoubleJump)
			{
				m_eState = Player_State::Walk;
			}
			m_pTransformCom->Set_Speed(5.f);
		}

		_vector vLook = m_pTransformCom->Get_State(State::Look);
		_vector vTest = vLook - vDirection;
		_float fInterpolTime = 0.4f;
		if (fabs((vTest).m128_f32[0]) > 0.05f or fabs((vTest).m128_f32[2]) > 0.05f)
		{
			if (not m_isInterpolating)
			{
				XMStoreFloat3(&m_vOriginalLook, vLook);
				m_isInterpolating = true;
			}

			if (m_fInterpolationRatio < fInterpolTime)
			{
				m_fInterpolationRatio += fTimeDelta;
			}
			else
			{
				m_fInterpolationRatio = fInterpolTime;
				m_isInterpolating = false;
			}

			_float fRatio = m_fInterpolationRatio / fInterpolTime;

			vDirection = XMVectorLerp(XMLoadFloat3(&m_vOriginalLook), vDirection, fRatio);
		}
		else
		{
			m_isInterpolating = false;
			m_fInterpolationRatio = 0.f;
		}

		m_pTransformCom->LookAt_Dir(vDirection);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}


	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		vDirection += vCamLook;
		m_pTransformCom->LookAt_Dir(vDirection);

		if (m_fAttTimer > 0.8f)
		{
			m_bAttacked = false;
		}

		m_eState = Player_State::Attack;
	}

	m_pTransformCom->Gravity(fTimeDelta);
}

void CPlayer::Customize(_float fTimeDelta)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -0.3f);
		}
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_MBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -0.3f);
		}
	}

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		if (m_Animation.iAnimIndex < Animation::End - 1)
		{
			m_Animation.iAnimIndex += 1;
		}
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		if (m_Animation.iAnimIndex > 0)
		{
			m_Animation.iAnimIndex -= 1;
		}
	}

	_uint LastBtnIndex = CUI_Manager::Get_Instance()->Get_sizeofButtons() - 1;
	if (CUI_Manager::Get_Instance()->is_Activated(LastBtnIndex))
	{
		if (m_Animation.iAnimIndex != Boruto_etc_Win_Type01_Loop)
		{
			m_Animation = {};
			m_Animation.iAnimIndex = Boruto_etc_Win_Type01_Start;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.5f;
			if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Boruto_etc_Win_Type01_Start))
			{
				m_Animation = {};
				m_Animation.iAnimIndex = Boruto_etc_Win_Type01_Loop;
				m_Animation.isLoop = true;
				m_Animation.bSkipInterpolation = true;
			}
		}
	}
	else
	{
		if (m_Animation.iAnimIndex == Boruto_etc_Win_Type01_Loop)
		{
			m_Animation = {};
			m_Animation.iAnimIndex = Boruto_etc_Win_Type01_End;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.bSkipInterpolation = true;
		}
		if (m_Animation.iAnimIndex == Boruto_etc_Win_Type01_Start or m_pBodyParts[PT_HEAD]->IsAnimationFinished(Boruto_etc_Win_Type01_End) or m_pBodyParts[PT_HEAD]->IsAnimationFinished(etc_Appearance))
		{
			m_Animation = {};
			m_Animation.iAnimIndex = CharaSelect_Idle;
			m_Animation.isLoop = true;
			m_Animation.fInterpolationTime = 0.6f;
		}
	}

}

void CPlayer::Apply_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::Player_State::Idle:
		m_Animation = {};
		m_Animation.iAnimIndex = Idle_Loop;
		m_Animation.isLoop = true;
		break;
	case Client::Player_State::Walk:
		m_Animation = {};
		m_Animation.iAnimIndex = Walk_Loop;
		m_Animation.isLoop = true;

		m_eState = Player_State::Walk_End;
		break;
	case Client::Player_State::Walk_End:
		m_Animation = {};
		m_Animation.iAnimIndex = Walk_End;
		m_Animation.isLoop = false;

		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Walk_End))
		{
			m_eState = Player_State::Idle;
		}
		break;
	case Client::Player_State::Run:
		m_Animation = {};
		m_Animation.iAnimIndex = Run_Loop;
		m_Animation.isLoop = true;

		m_eState = Player_State::Run_End;
		break;
	case Client::Player_State::Run_End:
		m_Animation = {};
		m_Animation.iAnimIndex = Run_End;
		m_Animation.isLoop = false;
		m_Animation.bSkipInterpolation = true;

		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Run_End))
		{
			m_eState = Player_State::Idle;
		}
		break;
	case Client::Player_State::Jump:
		m_Animation = {};
		m_Animation.iAnimIndex = Jump_Vertical;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 1.8f;

		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Jump_Vertical) or
			m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex() == Fall_Vertical_Loop)
		{
			m_Animation = {};
			m_Animation.iAnimIndex = Fall_Vertical_Loop;
			m_Animation.isLoop = true;
		}
		if (not m_pTransformCom->Is_Jumping())
		{
			m_eState = Player_State::Land;
		}
		break;
	case Client::Player_State::Jump_Front:
		m_Animation = {};
		m_Animation.iAnimIndex = Jump_Front;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 1.8f;

		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Jump_Front) or
			m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex() == Fall_Front_Loop)
		{
			m_Animation = {};
			m_Animation.iAnimIndex = Fall_Front_Loop;
			m_Animation.isLoop = true;
		}
		if (not m_pTransformCom->Is_Jumping())
		{
			m_eState = Player_State::Land;
		}
		break;
	case Client::Player_State::DoubleJump:
		m_Animation = {};
		m_Animation.iAnimIndex = DoubleJump;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 1.8f;
		m_Animation.bSkipInterpolation = true;

		if (not m_hasDoubleJumped)
		{
			m_hasDoubleJumped = true;
		}
		else if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(DoubleJump) or
				 m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex() == Fall_Front_Loop)
		{
			m_Animation = {};
			m_Animation.iAnimIndex = Fall_Front_Loop;
			m_Animation.isLoop = true;
		}

		if (not m_pTransformCom->Is_Jumping())
		{
			m_hasDoubleJumped = false;
			m_eState = Player_State::Land;
		}
		break;
	case Client::Player_State::Land:
		m_Animation = {};
		m_Animation.iAnimIndex = Land;
		m_Animation.isLoop = false;
		m_Animation.bSkipInterpolation = true;

		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Land))
		{
			m_eState = Player_State::Idle;
		}
		break;
	case Client::Player_State::Beaten:
		break;
	case Client::Player_State::Attack:
		if (not m_bAttacked)
		{
			if (m_fAttTimer > 0.8f)
			{
				if (m_iAttMotion > Sasuke_Attack_TurnSlashingShoulder_Right)
				{
					m_iAttMotion = Sasuke_Attack_DashSlashing_Right;
				}
				m_Animation = {};
				m_Animation.iAnimIndex = m_iAttMotion++;
				m_Animation.isLoop = false;
				m_Animation.fAnimSpeedRatio = 1.3f;

				m_fAttTimer = 0.f;
			}
			if (m_fAttTimer > 0.5f)
			{
				m_pGameInstance->Attack_Monster(m_pCollider_Att, 10);
				m_bAttacked = true;
			}
			else
			{
				m_pTransformCom->Set_Speed(0.5f);
				m_pTransformCom->Go_Straight(fTimeDelta);
			}
		}
		else if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(m_iAttMotion - 1))
		{
			m_eState = Player_State::Idle;
			m_bAttacked = false;
		}
		break;
	}
}

HRESULT CPlayer::Ready_Parts()
{
	BODYPART_DESC Desc;

	Desc.pParentTransform = m_pTransformCom;
	Desc.Animation = &m_Animation;

	Desc.eType = PT_HEAD;
	Desc.iNumVariations = 11;
	m_pBodyParts[PT_HEAD] = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BodyPart"), &Desc));
	if (not m_pBodyParts[PT_HEAD])
	{
		return E_FAIL;
	}

	Desc.eType = PT_FACE;
	Desc.iNumVariations = 3;
	m_pBodyParts[PT_FACE] = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BodyPart"), &Desc));
	if (not m_pBodyParts[PT_FACE])
	{
		return E_FAIL;
	}

	Desc.eType = PT_UPPER_BODY;
	Desc.iNumVariations = 16;
	m_pBodyParts[PT_UPPER_BODY] = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BodyPart"), &Desc));
	if (not m_pBodyParts[PT_UPPER_BODY])
	{
		return E_FAIL;
	}

	Desc.eType = PT_LOWER_BODY;
	Desc.iNumVariations = 11;
	m_pBodyParts[PT_LOWER_BODY] = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BodyPart"), &Desc));
	if (not m_pBodyParts[PT_LOWER_BODY])
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
#ifdef _DEBUG
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_vector vPos = m_pTransformCom->Get_State(State::Pos);
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), 2.f, 0.01f, 1.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pCollider_Att), &ColDesc)))
	{
		return E_FAIL;
	}

	ColDesc = {};
	ColDesc.eType = ColliderType::AABB;
	ColDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
	ColDesc.vCenter = _float3(0.f, ColDesc.vExtents.y, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pCollider_Hit), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (size_t i = 0; i < PT_END; i++)
	{
		Safe_Release(m_pBodyParts[i]);
	}
#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG

	Safe_Release(m_pCollider_Att);
	Safe_Release(m_pCollider_Hit);
}
