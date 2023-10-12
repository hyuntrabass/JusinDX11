#include "Level_Editor.h"

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
