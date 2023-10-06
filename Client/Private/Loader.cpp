#include "Loader.h"
#include "GameInstance.h"

#include "Loading_Objects.h"

CLoader::CLoader(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

_uint APIENTRY ThreadEntry(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Begin_Thread()))
	{
		return FALSE;
	}

	if (FAILED(pLoader->Loading_LevelResources()))
	{
		return FALSE;
	}

	if (FAILED(pLoader->End_Thread()))
	{
		return FALSE;
	}

	CoUninitialize();
	return TRUE;
}

HRESULT CLoader::Init(Level_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));

	if (!m_hThread)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Show_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());

	return S_OK;
}

_bool CLoader::isFinished()
{
	return m_isFinished;
}

HRESULT CLoader::Loading_LevelResources()
{
	HRESULT hr = 0;

	switch (m_eNextLevel)
	{
	case Client::Level_ID::Logo:
		hr = Load_Logo();
		break;
	case Client::Level_ID::Tutorial:
		hr = Load_Tutorial();
		break;
	case Client::Level_ID::Stage1:
		hr = Load_Stage1();
		break;
	case Client::Level_ID::Stage2:
		hr = Load_Stage2();
		break;
	}

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Logo()
{
	m_strLoadingText = L"Logo : Loading Texture";
#pragma region Texture
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Logo), TEXT("Prototype_Component_Texture_BackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/test%d.dds"), 2))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Model";
#pragma region Model
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Tutorial()
{
	m_strLoadingText = L"Tutorial : Loading Texture";
#pragma region Texture
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Model";
#pragma region Model
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Shader";
#pragma region Shader
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Prototype";
#pragma region Prototype
	for (size_t i = 0; i < 999999999; i++)
	{
		int a = 10;
	}
#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Stage1()
{
	m_strLoadingText = L"Stage1 : Loading Texture";
#pragma region Texture

#pragma endregion

	m_strLoadingText = L"Stage1 : Loading Model";
#pragma region Model

#pragma endregion

	m_strLoadingText = L"Stage1 : Loading Shader";
#pragma region Shader

#pragma endregion

	m_strLoadingText = L"Stage1 : Loading Prototype";
#pragma region Prototype

#pragma endregion

	m_strLoadingText = L"Stage1 : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Stage2()
{
	m_strLoadingText = L"Stage2 : Loading Texture";
#pragma region Texture

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Model";
#pragma region Model

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Shader";
#pragma region Shader

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Prototype";
#pragma region Prototype

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(_dev pDevice, _context pContext, Level_ID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
