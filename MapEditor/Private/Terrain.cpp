#include "Terrain.h"

CTerrain::CTerrain(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Init_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Init(void* pArg)
{
	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTerrain::Render()
{
	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	return S_OK;
}

CTerrain* CTerrain::Create(_dev pDevice, _context pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();
}
