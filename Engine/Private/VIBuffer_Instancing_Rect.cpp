#include "VIBuffer_Instancing_Rect.h"

CVIBuffer_Instancing_Rect::CVIBuffer_Instancing_Rect(_dev pDevice, _context pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Instancing_Rect::CVIBuffer_Instancing_Rect(const CVIBuffer_Instancing_Rect& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Instancing_Rect::Init_Prototype(_uint iNumInstances)
{
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof VTXPOSTEX;
	m_iNumVertices = 4;

	m_iNumInstances = iNumInstances;
	m_iIndexCountPerInstance = 6;
	m_iInstanceStride = sizeof VTXINSTANCING;

	m_iIndexStride = 2;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstances;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region Index
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint iIndex{};

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pIndices[iIndex++] = 0;
		pIndices[iIndex++] = 1;
		pIndices[iIndex++] = 2;
				 
		pIndices[iIndex++] = 0;
		pIndices[iIndex++] = 2;
		pIndices[iIndex++] = 3;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region Instance
	m_InstancingBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstancingBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstancingBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_InstancingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstancingBufferDesc.MiscFlags = 0;
	m_InstancingBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCING* pVertexInstance = new VTXINSTANCING[m_iNumInstances]{};

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pVertexInstance[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertexInstance[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertexInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertexInstance[i].vPos = _float4(0.f + i, 0.f, 0.f + i, 1.f);
	}

	m_InstancingInitialData.pSysMem = pVertexInstance;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Rect::Init(void* pArg)
{
	return m_pDevice->CreateBuffer(&m_InstancingBufferDesc, &m_InstancingInitialData, &m_pVBInstance);
}

CVIBuffer_Instancing_Rect* CVIBuffer_Instancing_Rect::Create(_dev pDevice, _context pContext, _uint iNumInstances)
{
	CVIBuffer_Instancing_Rect* pInstance = new CVIBuffer_Instancing_Rect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumInstances)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Instancing_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instancing_Rect::Clone(void* pArg)
{
	CVIBuffer_Instancing_Rect* pInstance = new CVIBuffer_Instancing_Rect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Instancing_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Rect::Free()
{
	__super::Free();
}
