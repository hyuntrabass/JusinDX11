#include "Camera_Debug.h"

CCamera_Debug::CCamera_Debug(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Debug::CCamera_Debug(const CCamera_Debug& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Debug::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Debug::Init(void* pArg)
{
	if (!pArg)
	{
		MSG_BOX("null Arg : CCamera_Debug");
		return E_FAIL;
	}

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Debug::Tick(_float fTimeDelta)
{
	CameraMode CamMode = static_cast<CameraMode>(m_pGameInstance->Get_CameraModeIndex());

	if (CamMode != CameraMode::Debug)
	{
		return;
	}

	if (m_pGameInstance->Key_Down('M'))
	{
		if (m_isMoveMode)
		{
			m_isMoveMode = false;
		}
		else
		{
			//POINT ptCenter = g_ptCenter;
			//ClientToScreen(g_hWnd, &ptCenter);
			SetCursorPos(g_ptCenter.x, g_ptCenter.y);
			m_isMoveMode = true;
		}
	}
	if (m_isMoveMode)
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * m_fMouseSensor);
			SetCursorPos(g_ptCenter.x, g_ptCenter.y);
		}

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta * dwMouseMove * m_fMouseSensor);
			SetCursorPos(g_ptCenter.x, g_ptCenter.y);
		}

		_float fSpeedRatio{};
		if (m_fSpeed < 1.5f)
		{
			fSpeedRatio = 0.2f;
		}
		else if (m_fSpeed < 10.f)
		{
			fSpeedRatio = 1.f;
		}
		else if (m_fSpeed < 50.f)
		{
			fSpeedRatio = 10.f;
		}
		else
		{
			fSpeedRatio = 30.f;
		}

		if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
		{
			m_fSpeed += fSpeedRatio;
		}
		else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
		{
			m_fSpeed -= fSpeedRatio;
			if (m_fSpeed < 0.7f)
			{
				m_fSpeed = 0.7f;
			}
		}

	}

	if (m_pGameInstance->Key_Pressing(VK_LSHIFT))
	{
		m_pTransformCom->Set_Speed(m_fSpeed * 2.f);
	}
	else if (m_pGameInstance->Key_Pressing(VK_LCONTROL))
	{
		m_pTransformCom->Set_Speed(m_fSpeed * 0.2f);
	}
	else
	{
		m_pTransformCom->Set_Speed(m_fSpeed);
	}

	if (m_pGameInstance->Key_Pressing('W'))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	__super::Tick(fTimeDelta);
}

void CCamera_Debug::Late_Tick(_float fTimeDelta)
{
}

CCamera_Debug* CCamera_Debug::Create(_dev pDevice, _context pContext)
{
	CCamera_Debug* pInstance = new CCamera_Debug(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Debug::Clone(void* pArg)
{
	CCamera_Debug* pInstance = new CCamera_Debug(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Debug::Free()
{
	__super::Free();
}
