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

HRESULT CLoader::Init(LEVEL_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));

	if (not m_hThread)
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
	case Client::LEVEL_LOGO:
		hr = Load_Logo();
		break;
	case Client::LEVEL_CREATECHARACTER:
		hr = Load_CreateCharacter();
		break;
	case Client::LEVEL_TUTORIAL:
		hr = Load_Tutorial();
		break;
	case Client::LEVEL_STAGE1:
		hr = Load_Stage1();
		break;
	case Client::LEVEL_BOSSSTAGE:
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
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/BackGround.jpg")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/Title.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StartBtn"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/TitleMenu_Btn%d.png"), 2))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_SubTitle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_UI_TitleMenu_Overlay_BC.png")))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"Logo : Loading Model";
#pragma region Model
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Effect/SM_EFF_Sphere_02.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroundBreak_A"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Effect/SM_EFF_GroundBreak_A.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroundBreak_B"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Effect/SM_EFF_GroundBreak_B.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	_matrix Pivot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_FlyingSphere"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Effect/SM_EFF_FlyingSphere.hyuntrastatmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Fireball"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Effect/SM_EFF_D32_B_01.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo"), CLogo::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_SubTitle"), CSubTitle::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Button_Common"), CButton_Common::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Logo : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_CreateCharacter()
{
	m_strLoadingText = L"CreateCharacter : Loading Texture";
#pragma region Texture
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_PartsWindow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CreateCharacter/customMenu.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_BackGround_CreateCharacter"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CreateCharacter/Background/T_UI_Fcility_Customize.dds")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_Title_Custom"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CreateCharacter/Title_Custom.dds")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_CustomBtn"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CreateCharacter/CustomButton%d.png"), 2))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"CreateCharacter : Loading Model";
#pragma region Model
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Pain"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Pain/Mesh/Pain.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	_uint iIndex{};
	string strInputFilePath = "../Bin/Resources/AnimMesh/Custom_w/Head/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_HEAD) + L"_" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	iIndex = {0};
	strInputFilePath = "../Bin/Resources/AnimMesh/Custom_w/Face/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_FACE) + L"_" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	iIndex = {0};
	strInputFilePath = "../Bin/Resources/AnimMesh/Custom_w/Lower_Body/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_LOWER_BODY) + L"_" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	iIndex = {0};
	strInputFilePath = "../Bin/Resources/AnimMesh/Custom_w/Upper_Body/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_UPPER_BODY) + L"_" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion

	m_strLoadingText = L"CreateCharacter : Loading Shader";
#pragma region Shader
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"CreateCharacter : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 300, 300))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BodyPart"), CBodyPart::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_PartsWindow"), CParts_Window::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BackGroundCC"), CBackGround_CreateCharacter::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Title_Custom"), CTitle_Custom::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"CreateCharacter : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Tutorial()
{
	m_strLoadingText = L"Tutorial : Loading Texture";
#pragma region Texture
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_LifeGauge_Base2.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_SP_Gauge_Base_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar_Base_Frame"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_SP_Gauge_Base_Frame_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Skill"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_Skill_Gauge_Base_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Skill_Frame"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_Skill_Gauge_Base2_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Tool"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_Tool_Gauge_Base_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlotBase_Tool_Frame"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hud/T_UI_Tool_Gauge_Base2_BC.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/Tile.dds")))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Model";
#pragma region Model
	// Model for Map Assets.
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Forest/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TUTORIAL, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	// Model for Map Collider.
	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Forest/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_COL_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TUTORIAL, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sky"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Sky/Mesh/Sky.hyuntrastatmesh",false, XMMatrixScaling(0.01f, 0.01f, 0.01f)))))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sandman"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Sandman/Mesh/SandNinja.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sandman"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Loser02/Mesh/Loser02.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Kakashi"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Kakashi/Mesh/Kakashi.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), CVIBuffer_Instancing_Point::Create(m_pDevice, m_pContext, 300))))

	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Shader";
#pragma region Shader
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex_Instancing.hlsl"), VTXPOINT_INSTANCING::Elements, VTXPOINT_INSTANCING::iNumElements))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Tutorial : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_MapCollider"), CMapCollider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_UI_HpBar"), CHpBar_Player::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_UI_HpBar_Base"), CHpBar_Player_Base::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_UI_SlotBase_Skill"), CSlotBase_Skill::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_UI_HpBar_SlotBase_Tool"), CSlotBase_Tool::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Sandman"), CSandman::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Kakashi"), CKakashi::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
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
	// Model for Map Assets.
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Village/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	// Model for Map Collider.
	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Village/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_COL_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
		}
	}

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
	// Model for Map Assets.
	string strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Cloud/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSSSTAGE, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
		}
	}

	// Model for Map Collider.
	strInputFilePath = "../Bin/Resources/StaticMesh/Maps/Cloud/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_COL_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSSSTAGE, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSSSTAGE, TEXT("Prototype_Model_Kurama"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Boss/Kurama/Mesh/Kurama.hyuntraanimmesh", false, XMMatrixScaling(0.8f, 0.8f, 0.8f)))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Shader";
#pragma region Shader

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Kurama"), CKurama::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_MiniBomb"), CMiniBomb::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_strLoadingText = L"Stage2 : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
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
