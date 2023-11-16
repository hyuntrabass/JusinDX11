#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Instancing::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Init(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
