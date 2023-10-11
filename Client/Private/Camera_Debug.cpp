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

	if (GetAsyncKeyState('M') * 0x8000)
	{
		if (m_isMoveMode)
		{
			m_isMoveMode = false;
		}
		else
		{
			POINT ptCenter = { static_cast<_long>(g_iWinSizeX * 0.5f), static_cast<_long>(g_iWinSizeY * 0.5f) };
			ClientToScreen(g_hWnd, &ptCenter);
			SetCursorPos(ptCenter.x, ptCenter.y);
			m_isMoveMode = true;
		}
	}
	if (m_isMoveMode)
	{
		GET_CURSOR_POINT(ptMouse);

		POINT ptCenter = { static_cast<_long>(g_iWinSizeX * 0.5f), static_cast<_long>(g_iWinSizeY * 0.5f) };

		_float fMouseMoveX = static_cast<_float>(ptMouse.x - ptCenter.x);
		_float fMouseMoveY = static_cast<_float>(ptMouse.y - ptCenter.y);

		ClientToScreen(g_hWnd, &ptCenter);

		if (fMouseMoveX)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fMouseMoveX * m_fMouseSensor);
			SetCursorPos(ptCenter.x, ptCenter.y);
		}

		if (fMouseMoveY)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta * fMouseMoveY * m_fMouseSensor);
			SetCursorPos(ptCenter.x, ptCenter.y);
		}
	}

	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (GetKeyState('D') & 0x8000)
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
