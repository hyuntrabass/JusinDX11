#include "Player.h"
#include "PhysX_Manager.h"
#include "BodyPart.h"

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

	m_Animation = { etc_Appearance, false };

	m_pTransformCom->Set_Speed(5.f);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	{
		return;
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CREATECHARACTER)
	{
		//if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(etc_Hand_Spray))
		//{
		//	m_Animation = { Idle_Loop, true };
		//}
		//m_pGameInstance->Apply_PhysX(m_pTransformCom);

		Move(fTimeDelta);

		//m_pGameInstance->Update_PhysX(m_pTransformCom);
	}
	else
	{
		Customize(fTimeDelta);
		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(etc_Appearance))
		{
			m_Animation = { CharaSelect_Idle, true };
		}
	}

	for (size_t i = 0; i < PT_END; i++)
	{
		m_pBodyParts[i]->Tick(fTimeDelta);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < PT_END; i++)
	{
		m_pBodyParts[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CPlayer::Render()
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
#endif // _DEBUG

	for (size_t i = 0; i < PT_END; i++)
	{
		if (FAILED(m_pBodyParts[i]->Render()))
		{
			return E_FAIL;
		}
	}

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


	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		m_pTransformCom->Set_Speed(15.f);
		m_isRunning = true;
	}
	else
	{
		m_pTransformCom->Set_Speed(5.f);
		m_isRunning = false;
	}

	_uint iCurrentAnimIndex = m_Animation.first;

	if (m_pGameInstance->Key_Down(DIK_SPACE) && iCurrentAnimIndex != DoubleJump)
	{
		if (m_pTransformCom->Is_Jumping())
		{
			m_Animation = { DoubleJump, false };
		}
		else if (hasMoved)
		{
			m_Animation = { Jump_Front, false };
		}
		else
		{
			m_Animation = { Jump_Vertical, false };
		}
		m_pTransformCom->Jump(15.f);
	}

	if (hasMoved /*&& iCurrentAnimIndex != Land*/)
	{
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

		m_pTransformCom->Look_At_Dir(vDirection);
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (not m_pTransformCom->Is_Jumping())
		{
			if (iCurrentAnimIndex != Jump_Front &&
				iCurrentAnimIndex != Jump_Vertical &&
				iCurrentAnimIndex != DoubleJump &&
				iCurrentAnimIndex != Fall_Vertical_Loop &&
				iCurrentAnimIndex != Fall_Front_Loop)
			{
				if (m_isRunning)
				{
					m_Animation = { Run_Loop, true };
					m_fSliding = 1.f;
				}
				else
				{
					m_Animation = { Walk_Loop, true };
				}
			}
		}
	}
	else if (not m_pTransformCom->Is_Jumping())
	{
		iCurrentAnimIndex = m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex();

		if (iCurrentAnimIndex == Run_Loop || iCurrentAnimIndex == Run_End)
		{
			if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Run_End))
			{
				m_Animation = { Idle_Loop, true };
			}
			else
			{
				m_Animation = { Run_End, false };
				m_pTransformCom->Set_Speed(15.f);
				if (m_fSliding > 0.f)
				{
					m_fSliding -= 0.02f;
				}
				m_pTransformCom->Go_Straight(fTimeDelta * m_fSliding);
			}
		}

		if (iCurrentAnimIndex == Walk_Loop || iCurrentAnimIndex == Walk_End)
		{
			if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Walk_End))
			{
				//m_pModelCom->Set_Animation(Idle_Loop, true);
				m_Animation = { Idle_Loop, true };
			}
			else
			{
				//m_pModelCom->Set_Animation(Walk_End);
				m_Animation = { Walk_End, false };
			}
		}
	}

	iCurrentAnimIndex = m_Animation.first;
	//iCurrentAnimIndex = m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex();

	if (iCurrentAnimIndex == Jump_Vertical || iCurrentAnimIndex == Fall_Vertical_Loop)
	{
		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Jump_Vertical))
		{
			m_Animation = { Fall_Vertical_Loop, true };
		}
		if (not m_pTransformCom->Is_Jumping())
		{
			m_Animation = { Land, false };
		}
	}
	else if (iCurrentAnimIndex == Jump_Front || iCurrentAnimIndex == Fall_Front_Loop || iCurrentAnimIndex == DoubleJump)
	{
		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Jump_Front) &&
			m_pBodyParts[PT_HEAD]->IsAnimationFinished(DoubleJump) &&
			iCurrentAnimIndex == m_pBodyParts[PT_HEAD]->Get_CurrentAnimationIndex())
		{
			m_Animation = { Fall_Front_Loop, true };
		}
		if (not m_pTransformCom->Is_Jumping())
		{
			m_Animation = { Land, false };
		}
	}
	else if (iCurrentAnimIndex == Land)
	{
		if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Land))
		{
			m_Animation = { Idle_Loop, true };
		}
	}

	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		m_Animation = { Sasuke_Attack_TurnKick_Right, false };
	}

	if (m_pBodyParts[PT_HEAD]->IsAnimationFinished(Sasuke_Attack_TurnKick_Right))
	{
		m_Animation = { Idle_Loop, true };
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
		if (m_Animation.first < Animation::End - 1)
		{
			m_Animation.first += 1;
		}
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		if (m_Animation.first > 0)
		{
			m_Animation.first -= 1;
		}
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
}
