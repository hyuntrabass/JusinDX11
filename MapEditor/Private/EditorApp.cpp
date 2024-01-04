#include "EditorApp.h"
#include "GameInstance.h"
#include "ImguiMgr.h"
#include "Terrain.h"
#include "Camera_Debug.h"
#include "Dummy.h"
#include "Sky.h"
#include "LoadingImg.h"
#include "3D_Cursor.h"

CEditorApp::CEditorApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY ThreadEntry(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
	{
		return FALSE;
	}

	CEditorApp* pApp = reinterpret_cast<CEditorApp*>(pArg);

	pApp->Begin_Thread();

	if (FAILED(pApp->Ready_Prototype_GameObject()))
	{
		return FALSE;
	}

	pApp->End_Thread();

	return TRUE;
}

HRESULT CEditorApp::Init()
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif // _DEBUG

	RECT rc{};
	GetClientRect(g_hWnd, &rc);

	GRAPHIC_DESC GraphicDesc;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iWinSizeX = rc.right - rc.left;
	GraphicDesc.iWinSizeY = rc.bottom - rc.top;
	GraphicDesc.isWindowed = TRUE;

	m_pGameInstance->Init_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext);

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Loading_Screen()))
	{
		return E_FAIL;
	}

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));
	if (!m_hThread)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEditorApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	m_fTimeAcc += fTimeDelta;
	m_fLoadingTime += fTimeDelta;

	m_strLoadingText = L"맵 에디터 로딩중";

	if (m_fLoadingTime >= 0.3f)
	{
		if (m_iNumDots > 2)
		{
			m_iNumDots = 0;
		}
		m_iNumDots++;

		m_fLoadingTime = 0.f;
	}
	for (size_t i = 0; i < m_iNumDots; i++)
	{
		m_strLoadingText += L".";
	}

	if (m_bLoadComplete)
	{
		if (!m_pImguiMgr)
		{
			(m_pImguiMgr = CImguiMgr::Get_Instance())->Init(m_pDevice, m_pContext, m_MapList, m_MapCOLList, &m_PropList);
			Safe_AddRef(m_pImguiMgr);
		}
	}

	m_pGameInstance->Tick_Engine(fTimeDelta);

	if (m_pImguiMgr)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m_pImguiMgr->Tick(fTimeDelta);
	}

}

HRESULT CEditorApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.7f, 0.7f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	if (!m_bLoadComplete)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", m_strLoadingText, _float2(200.f, 300.f), 1.f, Colors::Salmon);
	}

	if (m_pImguiMgr)
	{
		if (FAILED(m_pImguiMgr->Render()))
		{
			return E_FAIL;
		}
	}

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CEditorApp::Ready_Prototype_Component_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))

	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 300, 300))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEditorApp::Ready_Loading_Screen()
{
	if (FAILED(m_pGameInstance->Add_Font(L"Font_Malang", L"../../Client/Bin/Resources/Font/Naruto.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Loading_Screen", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Loading.dds"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(L"Prototype_GameObject_LoadingImg", CLoadingImg::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_LoadingImg", L"Prototype_GameObject_LoadingImg", &m_bLoadComplete)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEditorApp::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);
}

HRESULT CEditorApp::Ready_Prototype_GameObject()
{
#pragma region Light
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma region Texture
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Test/Tile1.dds")))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Loading/Loading_Icon.dds")))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region Model
	string strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Forest/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapList[0].push_back(strFileName);
			}
		}
	}
	// Model for Map Collider.
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Forest/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapCOLList[0].push_back(strFileName);
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Village/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapList[1].push_back(strFileName);
			}
		}
	}
	// Model for Map Collider.
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Village/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapCOLList[1].push_back(strFileName);
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Cloud/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapList[2].push_back(strFileName);
			}
		}
	}
	// Model for Map Collider.
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Cloud/COL_Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, true))))
			{
				return E_FAIL;
			}
			else
			{
				m_MapCOLList[2].push_back(strFileName);
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Maps/Props/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			m_iPropCount++;
			wstring strPrototypeTag = L"Prototype_Model_";
			wstring strFileName = entry.path().stem().wstring();
			string strFileNameString = entry.path().stem().string();
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag + strFileName, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath))))
			{
				return E_FAIL;
			}
			else
			{
				m_PropList.push_back(strFileNameString);
			}
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_SkySphere"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/StaticMesh/Sky/Mesh/Sky.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Kurama"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Boss/Kurama/Mesh/Kurama.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Kakashi"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Kakashi/Mesh/Kakashi.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sandman"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Loser02/Mesh/Loser02.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_SandNinja"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Sandman/Mesh/SandNinja.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/StaticMesh/Effect/Mesh/SM_EFF_Sphere_02.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}

	_matrix Scale = XMMatrixScaling(5.f, 5.f, 5.f);
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Trigger"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/StaticMesh/Effect/Mesh/SM_EFF_Sphere_02.mo.hyuntrastatmesh", false, Scale))))
	{
		return E_FAIL;
	}

#pragma endregion


#pragma region Prototype
	//if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dummy"), CDummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Cursor"), C3D_Cursor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

	m_bLoadComplete = true;

	return S_OK;
}

void CEditorApp::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);
}

CEditorApp* CEditorApp::Create()
{
	CEditorApp* pInstance = new CEditorApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CEditorApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditorApp::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pImguiMgr);
	CImguiMgr::Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
