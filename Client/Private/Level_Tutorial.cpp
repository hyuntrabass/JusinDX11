#include "Level_Tutorial.h"
#include "Level_Loading.h"

CLevel_Tutorial::CLevel_Tutorial(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Init()
{
	return S_OK;
}

void CLevel_Tutorial::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(VK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Stage1))))
		{
			return;
		}
	}
}

HRESULT CLevel_Tutorial::Render()
{
	return S_OK;
}

CLevel_Tutorial* CLevel_Tutorial::Create(_dev pDevice, _context pContext)
{
	CLevel_Tutorial* pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Tutorial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();
}
