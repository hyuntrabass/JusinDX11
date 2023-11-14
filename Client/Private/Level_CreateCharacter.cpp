#include "Level_CreateCharacter.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "UI_Manager.h"

CLevel_CreateCharacter::CLevel_CreateCharacter(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_CreateCharacter::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_CREATECHARACTER);

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Lights");
	}

	if (FAILED(Ready_Player()))
	{
		MSG_BOX("Failed to Ready Player");
	}

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		MSG_BOX("Failed to Ready Camera");
	}

	if (FAILED(CUI_Manager::Get_Instance()->Ready_UI_Custom()))
	{
		MSG_BOX("Failed to Ready UI_Tuto");
	}

	return S_OK;
}

void CLevel_CreateCharacter::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_PRIOR) || m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TUTORIAL))))
		{
			MSG_BOX("Failed to Open Level");
		}
		else
		{
			return;
		}
	}
}

HRESULT CLevel_CreateCharacter::Render()
{
	return S_OK;
}

HRESULT CLevel_CreateCharacter::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 5000.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Main"), &CamDesc)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Set_CameraModeIndex(CM_MAIN);

	return S_OK;
}

HRESULT CLevel_CreateCharacter::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -1.f, -2.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_CREATECHARACTER, LightDesc);
}

HRESULT CLevel_CreateCharacter::Ready_Player()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_CreateCharacter* CLevel_CreateCharacter::Create(_dev pDevice, _context pContext)
{
	CLevel_CreateCharacter* pInstance = new CLevel_CreateCharacter(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_CreateCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_CreateCharacter::Free()
{
	__super::Free();
}
