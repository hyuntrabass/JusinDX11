#include "Level_Logo.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(_dev* pDevice, _context* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Init()
{
	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_PRIOR) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(ToIndex(Level_ID::Loading), CLevel_Loading::Create(m_pDevice, m_pContext, Level_ID::Tutorial))))
		{
			return;
		}
	}
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(_dev* pDevice, _context* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
