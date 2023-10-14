#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, CGameInstance* pGameInstance)
	: m_hWnd(hWnd)
	, m_iWinSizeX(iWinSizeX)
	, m_iWinSizeY(iWinSizeY)
	, m_pGameInstance(pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
}

void CPicking::Tick()
{
	// ����Ʈ ���� Ŀ�� ��ġ
	POINT ptCursor{};
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

	// ���� �����̽� ���� ��ġ
	_vector vCursorPos = XMVectorSet
	(
		ptCursor.x / (m_iWinSizeX * 0.5f) - 1.f,
		ptCursor.y / (m_iWinSizeY * 0.5f) + 1.f,
		0.f,
		1.f
	);

	// �� �����̽� ���� ��ġ
	_matrix ProjMatrix_Inverse = m_pGameInstance->Get_Transform_Inversed(D3DTS::Proj);
	XMVector4Transform(vCursorPos, ProjMatrix_Inverse);

	_vector vRayPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	_vector vRayDir = vCursorPos - vRayPos;

	// ���� �����̽� ���� ��ġ
	_matrix ViewMat_Inverse = m_pGameInstance->Get_Transform_Inversed(D3DTS::View);
	XMStoreFloat4(&m_vRayPos_World, XMVector4Transform(vRayPos, ViewMat_Inverse));
	XMStoreFloat4(&m_vRayDir_World, XMVector4Normalize(XMVector4Transform(vRayDir, ViewMat_Inverse)));
}

CPicking* CPicking::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, CGameInstance* pGameInstance)
{
	return new CPicking(hWnd, iWinSizeX, iWinSizeY, pGameInstance);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
}
