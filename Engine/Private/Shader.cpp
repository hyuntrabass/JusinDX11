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
    _uint iHlslFlag{};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else  
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

    if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
    {
        return E_FAIL;
    }

    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (!pTechnique)
    {
        return E_FAIL;
    }

    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);

    m_iNumPasses = TechniqueDesc.Passes;

    for (size_t i = 0; i < m_iNumPasses; i++)
    {
        ID3D11InputLayout* pInputLayout{ nullptr };

        D3D11_INPUT_ELEMENT_DESC VertexElements[2] =
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, /* Position */
            { "Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, /* Texcoord */
        };

        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
        if (!pPass)
        {
            return E_FAIL;
        }

        D3DX11_PASS_DESC PassDesc{};
        pPass->GetDesc(&PassDesc);

        if (FAILED(m_pDevice->CreateInputLayout(VertexElements, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
        {
            return E_FAIL;
        }

        m_InputLayouts.push_back(pInputLayout);
    }

    return S_OK;
}

HRESULT CShader::Init(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);

    ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);
    if (!pPass)
    {
        return E_FAIL;
    }

    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

    pPass->Apply(0, m_pContext);

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

    for (auto& Layout : m_InputLayouts)
    {
        Safe_Release(Layout);
    }

    Safe_Release(m_pEffect);
}
