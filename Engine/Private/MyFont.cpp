#include "MyFont.h"

CMyFont::CMyFont(_dev pDevice, _context pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyFont::Init(const wstring& strFilePath)
{
    return S_OK;
}

HRESULT CMyFont::Render(const wstring& strText)
{
    return S_OK;
}

CMyFont* CMyFont::Create(_dev pDevice, _context pContext, const wstring& strFilePath)
{
    CMyFont* pInstance = new CMyFont(pDevice, pContext);

    if (FAILED(pInstance->Init(strFilePath)))
    {
        MSG_BOX("Failed to Create : CMyFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyFont::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
