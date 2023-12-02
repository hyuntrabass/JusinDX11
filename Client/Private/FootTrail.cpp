#include "FootTrail.h"

CFootTrail::CFootTrail(_dev pDevice, _context pContext)
    : CBlendObject(pDevice, pContext)
{
}

CFootTrail::CFootTrail(const CFootTrail& rhs)
    : CBlendObject(rhs)
{
}

HRESULT CFootTrail::Init_Prototype()
{
    return S_OK;
}

HRESULT CFootTrail::Init(void* pArg)
{
    return S_OK;
}

void CFootTrail::Tick(_float3 vPos, _float fTimeDelta)
{
    if (m_TrailPosList.size() >= 50)
    {
    	m_TrailPosList.pop_back();
    }
    m_TrailPosList.push_front(vPos);

}

void CFootTrail::Late_Tick(_float fTimeDelta)
{
}

HRESULT CFootTrail::Render()
{
    return E_NOTIMPL;
}

HRESULT CFootTrail::Add_Components()
{


    return S_OK;
}

HRESULT CFootTrail::Bind_ShaderResources()
{
    return E_NOTIMPL;
}

CFootTrail* CFootTrail::Create(_dev pDevice, _context pContext)
{
    return nullptr;
}

CGameObject* CFootTrail::Clone(void* pArg)
{
    return nullptr;
}

void CFootTrail::Free()
{
}
