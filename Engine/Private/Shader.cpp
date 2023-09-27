#include "Shader.h"

CShader::CShader(_dev pDevice, _context pContext)
    : CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
    : CComponent(rhs)
{
}

HRESULT CShader::Init_Prototype(const wstring& strShaderFilePath)
{
    return S_OK;
}

HRESULT CShader::Init(void* pArg)
{
    return S_OK;
}

CShader* CShader::Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype(strShaderFilePath)))
    {
        MSG_BOX("Failed to Create : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
