#include "EditorApp.h"
#include "GameInstance.h"
#include "ImguiMgr.h"
#include "Terrain.h"
#include "Camera_Debug.h"
#include "Dummy.h"
#include "Sky.h"

CEditorApp::CEditorApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
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

	m_pGameInstance->Init_Engine(ToIndex(Level_ID::End), GraphicDesc, &m_pDevice, &m_pContext);

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObject()))
	{
		return E_FAIL;
	}

	m_pImguiMgr = CImguiMgr::Create(m_pDevice, m_pContext, m_pGameInstance);

	return S_OK;
}

void CEditorApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	m_fTimeAcc += fTimeDelta;

	m_pGameInstance->Tick_Engine(fTimeDelta);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pImguiMgr->Tick();

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

	if (FAILED(m_pImguiMgr->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CEditorApp::Ready_Prototype_Component_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))

	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 300, 300))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxCubeTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxStatMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEditorApp::Ready_Prototype_GameObject()
{
#pragma region Light
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(ToIndex(Level_ID::Static), LightDesc)))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma region Texture
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Test/Tile1.dds")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Dummy"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Test/Dummy.dds")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Cursor"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Loading/Loading_Icon.dds")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sky.dds")))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region Model
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Sphere"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Sky.hyntrastatmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Building_A"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_AreaA_Building_A.hyntrastatmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_01"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_01.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_02"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_02.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_03"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_03.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_04"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_04.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_05"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_05.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_06"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_06.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_07"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_07.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_08"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_08.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_09"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_09.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_10"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_10.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_11"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_11.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_12"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_12.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_13"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_13.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Konohavill_Ground_14"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/konohavill_ground_14.hyntrastatmesh"))))
	{
		return E_FAIL;
	}
#pragma endregion


#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

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

#pragma endregion

	return S_OK;
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
	Safe_Release(m_pImguiMgr);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
