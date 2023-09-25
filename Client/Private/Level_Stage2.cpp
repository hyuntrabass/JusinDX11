#include "Level_Stage2.h"

CLevel_Stage2::CLevel_Stage2(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Init()
{
	return S_OK;
}

void CLevel_Stage2::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_Stage2::Render()
{
	return S_OK;
}

CLevel_Stage2* CLevel_Stage2::Create(_dev pDevice, _context pContext)
{
	CLevel_Stage2* pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Stage2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();
}
