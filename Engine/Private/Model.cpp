#include "Model.h"
#include "Static_Mash.h"

CModel::CModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
{
	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}
}

HRESULT CModel::Init_Prototype(const string& strFilePath)
{
	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		ModelFile.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
		m_iFilePos = ModelFile.tellg();
		ModelFile.close();
	}
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CStatic_Mesh* pMesh = CStatic_Mesh::Create(m_pDevice, m_pContext, strFilePath, &m_iFilePos);
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Init(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& pMesh : m_Meshes)
	{
		if (pMesh)
		{
			pMesh->Render();
		}
	}

	return S_OK;
}

CModel* CModel::Create(_dev pDevice, _context pContext, const string& strFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath)))
	{
		MSG_BOX("Failed to Create : CModel");
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
