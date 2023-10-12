#include "Input_Manager.h"

CInput_Manager::CInput_Manager()
{
}

_bool CInput_Manager::Key_Pressing(_uint iKey)
{
	if (GetAsyncKeyState(iKey) & 0x8000)
		return true;

	return false;
}

_bool CInput_Manager::Key_Down(_uint iKey, InputChannel eInputChannel)
{
	if (!m_bKeyState[ToIndex(eInputChannel)][iKey] && (GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[ToIndex(eInputChannel)][iKey] = !m_bKeyState[ToIndex(eInputChannel)][iKey];
		return true;
	}

	if (m_bKeyState[ToIndex(eInputChannel)][iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
		m_bKeyState[ToIndex(eInputChannel)][iKey] = !m_bKeyState[ToIndex(eInputChannel)][iKey];

	return false;
}

_bool CInput_Manager::Key_Up(_uint iKey, InputChannel eInputChannel)
{
	if (m_bKeyState[ToIndex(eInputChannel)][iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[ToIndex(eInputChannel)][iKey] = !m_bKeyState[ToIndex(eInputChannel)][iKey];
		return true;
	}

	if (!m_bKeyState[ToIndex(eInputChannel)][iKey] && (GetAsyncKeyState(iKey) & 0x8000))
		m_bKeyState[ToIndex(eInputChannel)][iKey] = !m_bKeyState[ToIndex(eInputChannel)][iKey];


	return false;
}

CInput_Manager* CInput_Manager::Create()
{
	return new CInput_Manager();
}

void CInput_Manager::Free()
{
}
