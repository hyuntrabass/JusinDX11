#include "Dummy.h"

CDummy::CDummy(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CDummy::CDummy(const CDummy& rhs)
    : CGameObject(rhs)
{
}

HRESULT CDummy::Init_Prototype()
{
    return S_OK;
}

HRESULT CDummy::Init(void* pArg)
{
    if (FAILED(Add_Components()))
    {
        return E_FAIL;
    }

    DummyInfo Info = *(DummyInfo*)pArg;

    m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
    m_pTransformCom->Look_At_Dir(XMLoadFloat4(&Info.vLook));
    
    return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
}

void CDummy::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CDummy::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Begin(0)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render()))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CDummy::Add_Components()
{
    if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxCubeTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Dummy"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
    {
        return E_FAIL;
    }

    return S_OK;
}

CDummy* CDummy::Create(_dev pDevice, _context pContext)
{
    CDummy* pInstance = new CDummy(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CDummy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
    CDummy* pInstance = new CDummy(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CDummy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDummy::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
