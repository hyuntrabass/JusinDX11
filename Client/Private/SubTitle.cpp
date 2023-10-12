#include "SubTitle.h"

CSubTitle::CSubTitle(_dev pDevice, _context pContext)
    : COrthographicObject(pDevice, pContext)
{
}

CSubTitle::CSubTitle(const CSubTitle& rhs)
    : COrthographicObject(rhs)
{
}

HRESULT CSubTitle::Init_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CSubTitle::Init(void* pArg)
{
    return E_NOTIMPL;
}

void CSubTitle::Tick(_float fTimeDelta)
{
}

void CSubTitle::Late_Tick(_float fTimeDelta)
{
}

HRESULT CSubTitle::Render()
{
    return E_NOTIMPL;
}

HRESULT CSubTitle::Add_Components()
{
    return E_NOTIMPL;
}

HRESULT CSubTitle::Bind_ShaderResources()
{
    return E_NOTIMPL;
}

CSubTitle* CSubTitle::Create(_dev pDevice, _context pContext)
{
    return nullptr;
}

CGameObject* CSubTitle::Clone(void* pArg)
{
    return nullptr;
}

void CSubTitle::Free()
{
}
