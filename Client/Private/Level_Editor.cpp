#include "Level_Editor.h"
#include "Camera_Debug.h"

CLevel_Editor::CLevel_Editor(_dev pDevice, _context pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Init()
{
    if (FAILED(Ready_Layer()))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CLevel_Editor::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_Editor::Render()
{
    return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer()
{
    if (!m_pGameInstance)
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Editor), TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
    {
        return E_FAIL;
    }

    CCamera::Camera_Desc CamDesc;
    CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
    CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 0.f);
    CamDesc.fFovY = XMConvertToRadians(90.f);
    CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
    CamDesc.fNear = 0.1f;
    CamDesc.fFar = 300.f;

    if (FAILED(m_pGameInstance->Add_Layer(ToIndex(Level_ID::Editor), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CLevel_Editor* CLevel_Editor::Create(_dev pDevice, _context pContext)
{
    CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);

    if (FAILED(pInstance->Init()))
    {
        MSG_BOX("Failed to Create : CLevel_Editor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Editor::Free()
{
    __super::Free();
}
