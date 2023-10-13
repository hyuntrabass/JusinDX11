#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

_float4 CPipeLine::Get_CameraPos() const
{
	return m_vCameraPos;
}

_float4x4 CPipeLine::Get_Transform_Float4x4(D3DTS eState) const
{
	return m_TransformMatrix[ToIndex(eState)];
}

_float4x4 CPipeLine::Get_Transform_Inversed_Float4x4(D3DTS eState) const
{
	return m_TransformMatrix_Inversed[ToIndex(eState)];
}

_matrix CPipeLine::Get_Transform(D3DTS eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[ToIndex(eState)]);
}

_matrix CPipeLine::Get_Transform_Inversed(D3DTS eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inversed[ToIndex(eState)]);
}

void CPipeLine::Set_Transform(D3DTS eState, const _float4x4& TransformMatrix)
{
	m_TransformMatrix[ToIndex(eState)] = TransformMatrix;
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[ToIndex(eState)],TransformMatrix);
}

HRESULT CPipeLine::Init()
{
	for (size_t i = 0; i < ToIndex(D3DTS::End); i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrix_Inversed[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < ToIndex(D3DTS::End); i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inversed[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}
	m_vCameraPos = _float4(&m_TransformMatrix_Inversed[ToIndex(D3DTS::View)]._41);
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();
	
	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
}
