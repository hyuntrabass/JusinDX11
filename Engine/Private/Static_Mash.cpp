#include "Static_Mash.h"

CStatic_Mesh::CStatic_Mesh(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CStatic_Mesh::Init_Prototype(const string& strFilePath, streampos* iFilePos)
{
	ifstream MeshFile(strFilePath.c_str(), ios::binary);

	if (MeshFile.fail())
	{
		return E_FAIL;
	}
	MeshFile.seekg(*iFilePos);
	_uint iNameSize{};
	MeshFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	m_pName = new char[iNameSize];
	MeshFile.read(m_pName, iNameSize);
	MeshFile.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof _uint);
	_uint iNumFaces{};
	MeshFile.read(reinterpret_cast<_char*>(&iNumFaces), sizeof _uint);

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXSTATICMESH;

	m_iIndexStride = 4;
	m_iNumIndices = iNumFaces * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
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

	VTXSTATICMESH* pVertices = new VTXSTATICMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		MeshFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
		MeshFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
		MeshFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);
		MeshFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);
	}


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

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_ulong dwIndex{};
	for (size_t i = 0; i < iNumFaces; i++)
	{
		MeshFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex++]), sizeof _uint);
		MeshFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex++]), sizeof _uint);
		MeshFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex++]), sizeof _uint);
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}
	*iFilePos = MeshFile.tellg();
	MeshFile.close();
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CStatic_Mesh::Init(void* pArg)
{
	return S_OK;
}

CStatic_Mesh* CStatic_Mesh::Create(_dev pDevice, _context pContext, const string& strFilePath, streampos* iFilePos)
{
	CStatic_Mesh* pInstance = new CStatic_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, iFilePos)))
	{
		MSG_BOX("Failed to Create : CStatic_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStatic_Mesh::Clone(void* pArg)
{
	CStatic_Mesh* pInstance = new CStatic_Mesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CStatic_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatic_Mesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pName);
}
