#include "Camera_Main.h"
#include "UI_Manager.h"
#include "Trigger_Manager.h"

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
	Safe_AddRef(m_pPlayerTransform);
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

	m_pTrigger_Manager = CTrigger_Manager::Get_Instance();
	Safe_AddRef(m_pTrigger_Manager);

	//m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);

	return S_OK;
}

void CCamera_Main::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() != CM_MAIN)
	{
		return;
	}
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pGameInstance->Set_CameraModeIndex(CM_DEBUG);
	}

	if (m_pTrigger_Manager->Hasto_PlayScene())
	{
		if (not m_pScene)
		{
			m_pScene = m_pTrigger_Manager->Get_CurrentScene();
			m_iSceneIndex = {};
			m_fSceneTimer = {};
		}

		m_pTransformCom->Set_State(State::Pos, XMVectorLerp(XMLoadFloat4(&(*m_pScene)[m_iSceneIndex].first), XMLoadFloat4(&(*m_pScene)[m_iSceneIndex + 1].first), m_fSceneTimer * 4.f));
		m_pTransformCom->LookAt_Dir(XMVectorLerp(XMLoadFloat4(&(*m_pScene)[m_iSceneIndex].second), XMLoadFloat4(&(*m_pScene)[m_iSceneIndex + 1].second), m_fSceneTimer * 4.f));

		m_fSceneTimer += fTimeDelta;
		if (m_fSceneTimer > 0.25f)
		{
			m_fSceneTimer = {};
			m_iSceneIndex++;
			if (m_iSceneIndex >= m_pScene->size() - 1)
			{
				m_pTrigger_Manager->End_Scene();
				m_pScene = nullptr;
			}
		}

		__super::Tick(fTimeDelta);
		return;
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

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		m_fPlayerDistance -= 1.f;
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		m_fPlayerDistance += 1.f;
	}

	// 	y = sin(x * 10.0f) * powf(0.5f, x)

	if (m_pGameInstance->Get_ShakeCam())
	{
		m_fShakeAcc = {0.1f};
		m_pGameInstance->Set_ShakeCam(false);
	}

	_float fShakeAmount = sin(m_fShakeAcc * 15.f) * powf(0.5f, m_fShakeAcc) * 0.2f;

	m_pTransformCom->Set_State(State::Pos, 
							   m_pPlayerTransform->Get_CenterPos() 
							   - (m_pTransformCom->Get_State(State::Look) * m_fPlayerDistance)
							   + (m_pTransformCom->Get_State(State::Up) * m_fPlayerDistance * 0.25f));

	PxRaycastBuffer Buffer{};
	_vector vRayDir = m_pTransformCom->Get_State(State::Pos) - m_pPlayerTransform->Get_CenterPos();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
	if (m_pGameInstance->Raycast(m_pPlayerTransform->Get_CenterPos() + XMVector3Normalize(vRayDir) * .5f, XMVector3Normalize(vRayDir), fDist, Buffer))
	{
		m_pTransformCom->Set_State(State::Pos, PxVec3ToVector(Buffer.block.position, 1.f));
	}

	_vector vShakePos = m_pTransformCom->Get_State(State::Pos);
	vShakePos += XMVectorSet(fShakeAmount, -fShakeAmount, 0.f, 0.f);
	m_pTransformCom->Set_State(State::Pos, vShakePos);

	m_fShakeAcc += fTimeDelta * 10.f;
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

	Safe_Release(m_pTrigger_Manager);
	Safe_Release(m_pPlayerTransform);
}
