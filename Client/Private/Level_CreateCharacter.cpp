#include "Level_CreateCharacter.h"
#include "Level_Loading.h"
#include "Camera.h"

CLevel_CreateCharacter::CLevel_CreateCharacter(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_CreateCharacter::Init()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_CreateCharacter::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_PRIOR) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Tutorial))))
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
	CamDesc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::CreateCharacter), strLayerTag, TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::CreateCharacter), strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
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
