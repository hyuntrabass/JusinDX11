#include "Level_Stage1.h"
#include "Level_Loading.h"

CLevel_Stage1::CLevel_Stage1(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Init()
{
	return S_OK;
}

void CLevel_Stage1::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_PRIOR) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Stage2))))
		{
			return;
		}
	}
}

HRESULT CLevel_Stage1::Render()
{
	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(_dev pDevice, _context pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Stage1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();
}
