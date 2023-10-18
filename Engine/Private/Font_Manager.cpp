#include "Font_Manager.h"
#include "MyFont.h"

CFont_Manager::CFont_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Init()
{
	return S_OK;
}

CFont_Manager* CFont_Manager::Create(_dev pDevice, _context pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CFont_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
	{
		Safe_Release(Pair.second);
	}
	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
