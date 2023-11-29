#include "Camera_Main.h"
#include "UI_Manager.h"

CCamera_Main::CCamera_Main(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Main::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Main::Init(void* pArg)
{
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (not m_pTransformCom)
	{
		MSG_BOX("Can't Find Player!! : Camera Main");
	}

	if (not pArg)
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

void CCamera_Main::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() != CM_MAIN)
	{
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pGameInstance->Set_CameraModeIndex(CM_DEBUG);
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CREATECHARACTER)
	{
		_uint iPageIndex = CUI_Manager::Get_Instance()->Get_PageIndex();
		switch (iPageIndex)
		{
		case 1: // 머리, 얼굴
		case 2:
			m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.042009832f, 1.62391f, 0.535985f, 1.f));
			m_pTransformCom->LookAt_Dir(XMVectorSet(0.332016f, -0.050258351f, -0.941938f, 0.f));
			break;
		case 3: // 상의
			m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.108874832f, 1.32057f, 1.113212f, 1.f));
			m_pTransformCom->LookAt_Dir(XMVectorSet(0.306376f, -0.095230853f, -0.947139f, 0.f));
			break;
		case 4: // 하의
			m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.416429432f, 0.817432f, 1.532582f, 1.f));
			m_pTransformCom->LookAt_Dir(XMVectorSet(0.2184516f, -0.155187153f, -0.963422f, 0.f));
			break;
		default:
			m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.f, 1.5f, 2.5f, 1.f));
			m_pTransformCom->LookAt_Dir(XMVectorSet(0.365148f, -0.182574f, -0.912871f, 0.f));
			break;
		}

		__super::Tick(fTimeDelta);
		return;
	}

	_long dwMouseMove;

	if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
	}

	if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
	}


	_float fRStickMove{};
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().x)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * fRStickMove);
	}
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().y)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * -fRStickMove);
	}

	m_pTransformCom->Set_State(State::Pos, m_pPlayerTransform->Get_State(State::Pos) - m_pTransformCom->Get_State(State::Look) * 3.f + XMVectorSet(0.f, 2.f, 0.f, 0.f));

	__super::Tick(fTimeDelta);
}

void CCamera_Main::Late_Tick(_float fTimeDelta)
{
}

CCamera_Main* CCamera_Main::Create(_dev pDevice, _context pContext)
{
	CCamera_Main* pInstance = new CCamera_Main(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Main::Clone(void* pArg)
{
	CCamera_Main* pInstance = new CCamera_Main(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();
}
