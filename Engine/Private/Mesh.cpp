#include "Mesh.h"
#include "GameInstance.h"

CMesh::CMesh(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Init_Prototype(ifstream& ModelFile, ModelType eType, _fmatrix OffsetMatrix)
{
	ModelFile.read(reinterpret_cast<_char*>(&m_iMatIndex), sizeof _uint);

	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	m_pName = new char[iNameSize];
	ModelFile.read(m_pName, iNameSize);
	ModelFile.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof _uint);
	_uint iNumFaces{};
	ModelFile.read(reinterpret_cast<_char*>(&iNumFaces), sizeof _uint);

	m_pVerticesPos = new _float3[m_iNumVertices];
	m_pVerticesNor = new _float3[m_iNumVertices];

	m_iNumVertexBuffers = 1;

	m_iIndexStride = 4;
	m_iNumIndices = iNumFaces * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
	switch (eType)
	{
	case Engine::ModelType::Static:
		Ready_StaticMesh(ModelFile, OffsetMatrix);
		break;
	case Engine::ModelType::Anim:
		Ready_AnimMesh(ModelFile);
		break;
	}
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
	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_ulong dwIndex{};
	for (size_t i = 0; i < iNumFaces; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex] = pIndices[dwIndex++];
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex] = pIndices[dwIndex++];
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex] = pIndices[dwIndex++];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Init(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_StaticMesh(ifstream& ModelFile, _fmatrix OffsetMatrix)
{
	m_iVertexStride = sizeof VTXSTATICMESH;
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
		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), OffsetMatrix));
		m_pVerticesPos[i] = pVertices[i].vPosition;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), OffsetMatrix));
		m_pVerticesNor[i] = pVertices[i].vNormal;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CMesh::Ready_AnimMesh(ifstream& ModelFile)
{
	m_iVertexStride = sizeof VTXANIMMESH;
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
		m_pVerticesPos[i] = pVertices[i].vPosition;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
		m_pVerticesNor[i] = pVertices[i].vNormal;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vBlendIndices), sizeof XMUINT4);
		
		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vBlendWeights), sizeof _float4);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);


	return S_OK;
}

_bool CMesh::Intersect_RayMesh(_fmatrix WorldMatrix, _float4* pPickPos)
{
	_uint Index[3]{};
	_float3 vPickPos{};

	m_pGameInstance->TransformRay_ToLocal(WorldMatrix);

	for (size_t i = 0; i < m_iNumIndices / 3; i++)
	{
		Index[0] = i * 3;
		Index[1] = (i * 3) + 1;
		Index[2] = (i * 3) + 2;

		_vector Vertices[3]
		{
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[0]]]),
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[1]]]),
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[2]]])
		};

		//if (!BoundingFrustum(m_pGameInstance->Get_Transform(D3DTS::Proj)).Intersects(Vertices[0], Vertices[1], Vertices[2]))
		//{
		//	return false;
		//}
		_vector Normal = XMVector3Normalize(XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[0]]]) +
											XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[1]]]) +
											XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[2]]]));

		if (m_pGameInstance->Picking_InLocal(Vertices[0],
											 Vertices[1],
											 Vertices[2],
											 Normal, pPickPos))
		{
			return true;
		}
	}

	return false;
}

CMesh* CMesh::Create(_dev pDevice, _context pContext, ifstream& ModelFile, ModelType eType, _fmatrix OffsetMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(ModelFile, eType, OffsetMatrix)))
	{
		MSG_BOX("Failed to Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pName);
}
