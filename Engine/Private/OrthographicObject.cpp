#include "OrthographicObject.h"

COrthographicObject::COrthographicObject(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

COrthographicObject::COrthographicObject(const COrthographicObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT COrthographicObject::Init_Prototype()
{
	return S_OK;
}

HRESULT COrthographicObject::Init(void* pArg)
{
	return S_OK;
}

void COrthographicObject::Tick(_float fTimeDelta)
{
}

void COrthographicObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT COrthographicObject::Render()
{
	return S_OK;
}

void COrthographicObject::Apply_Orthographic(_uint iWinSizeX, _uint iWinSizeY, _float fPosZ)
{
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(State::Pos, XMVectorSet(m_fX - iWinSizeX * 0.5f, -m_fY + iWinSizeY * 0.5f, fPosZ, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(iWinSizeX, iWinSizeY, 0.f, 1.1f));
}

void COrthographicObject::Free()
{
	__super::Free();
}
