#include "RenderTarget.h"

CRenderTarget::CRenderTarget(_dev pDevice, _context pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

ID3D11RenderTargetView* CRenderTarget::Get_RenderTargetView()
{
    return m_pRenderTargetView;
}

HRESULT CRenderTarget::Init(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat)
{
    D3D11_TEXTURE2D_DESC Desc{};
    Desc.Width = iWidth;
    Desc.Height = iHeight;
    Desc.MipLevels = 1;
    Desc.ArraySize = 1;
    Desc.Format = ePixelFormat;

    Desc.SampleDesc.Quality = 0;
    Desc.SampleDesc.Count = 1;

    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pTexture)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRenderTargetView)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pShaderResourceView)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CRenderTarget* CRenderTarget::Create(_dev pDevice, _context pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);
    
    if (FAILED(pInstance->Init(iWidth, iHeight, ePixelFormat)))
    {
        MSG_BOX("Failed to Create : CRenderTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    SaveDDSTextureToFile(m_pContext, m_pTexture, TEXT("../Bin/Diffuse.dds"));

    Safe_Release(m_pRenderTargetView);
    Safe_Release(m_pShaderResourceView);

    Safe_Release(m_pTexture);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
