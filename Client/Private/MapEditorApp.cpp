#include "MapEditorApp.h"
#include "GameInstance.h"
#include "ImguiMgr.h"
#include "Level_Loading.h"
#include "Loading_Icon.h"
#include "Loading_Screen.h"

CMapEditorApp::CMapEditorApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapEditorApp::Init()
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

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

	if (FAILED(Ready_Prototype_For_Loading()))
	{
		return E_FAIL;
	}

	m_pImguiMgr = CImguiMgr::Create(m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Editor))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMapEditorApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	m_fTimeAcc += fTimeDelta;

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMapEditorApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 1.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	if (FAILED(m_pImguiMgr->DrawEditor()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMapEditorApp::Ready_Prototype_Component_For_Static()
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 300, 300))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapEditorApp::Ready_Prototype_For_Loading()
{
#pragma region Textures
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Loading_Screen"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/LoadingScreen_%d.dds"), 7))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Loading_Indicator"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Indicator.dds")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Loading_Icon"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Icon.dds")))))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Loading_Screen"), CLoading_Screen::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Loading_Icon"), CLoading_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

CMapEditorApp* CMapEditorApp::Create()
{
	CMapEditorApp* pInstance = new CMapEditorApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CMapEditorApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapEditorApp::Free()
{
	Safe_Release(m_pImguiMgr);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
