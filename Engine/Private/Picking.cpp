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
	// 뷰포트 상의 커서 위치
	POINT ptCursor{};
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

	// 투영 스페이스 상의 위치
	_vector vCursorPos
	{
		XMVectorSet
		(
			ptCursor.x / (m_iWinSizeX * 0.5f) - 1.f,
			ptCursor.y / (m_iWinSizeY * -0.5f) + 1.f,
			0.f,
			1.f
		)
	};

	// 뷰 스페이스 상의 위치
	_matrix ProjMatrix_Inverse{ m_pGameInstance->Get_Transform_Inversed(D3DTS::Proj) };
	XMVector4Transform(vCursorPos, ProjMatrix_Inverse);

	_vector vRayPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	_vector vRayDir{ vCursorPos - vRayPos };

	// 월드 스페이스 상의 위치
	_matrix ViewMat_Inverse{ m_pGameInstance->Get_Transform_Inversed(D3DTS::View) };
	XMStoreFloat4(&m_vRayPos_World, XMVector4Transform(vRayPos, ViewMat_Inverse));
	XMStoreFloat4(&m_vRayDir_World, XMVector4Normalize(XMVector4Transform(vRayDir, ViewMat_Inverse)));
}

const _bool& CPicking::Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* vPickPos)
{
	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_World) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_World) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat3(vPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

CPicking* CPicking::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, CGameInstance* pGameInstance)
{
	return new CPicking(hWnd, iWinSizeX, iWinSizeY, pGameInstance);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
}
