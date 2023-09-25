#include "BlendObject.h"

CBlendObject::CBlendObject(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlendObject::CBlendObject(const CBlendObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlendObject::Init_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Init(void* pArg)
{
	return S_OK;
}

void CBlendObject::Tick(_float fTimeDelta)
{
}

void CBlendObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

HRESULT CBlendObject::Compute_CamDistance(CTransform* pTransform)
{
	return S_OK;
}

void CBlendObject::Free()
{
	__super::Free();
}
