#include "Model.h"
#include "Static_Mash.h"
#include "Texture.h"

CModel::CModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
{
	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_AddRef(pTexture);
		}
	}
}

const _uint& CModel::Get_NumMeshes() const
{
	return m_iNumMeshes;
}

HRESULT CModel::Init_Prototype(const string& strFilePath, _fmatrix OffsetMatrix)
{
	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		ModelFile.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
		m_iFilePos = ModelFile.tellg();
		m_Meshes.reserve(m_iNumMeshes);

		// 매쉬 로드
		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			CStatic_Mesh* pMesh = CStatic_Mesh::Create(m_pDevice, m_pContext, strFilePath, &m_iFilePos, OffsetMatrix);
			m_Meshes.push_back(pMesh);
		}

		// 메테리얼 로드
		_char szMatFilePath[MAX_PATH]{};
		_char szFullPath[MAX_PATH]{};

		_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
		strcat_s(szMatFilePath, "../Texture/");

		ModelFile.seekg(m_iFilePos);
		ModelFile.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

		for (size_t i = 0; i < m_iNumMaterials; i++)
		{
			Model_Material Material{};
			_uint iNameSize{};
			_char* pFileName{};

			for (size_t j = 0; j < ToIndex(TextureType::End); j++)
			{
				ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
				if (iNameSize == 1)
				{
					continue;
				}

				pFileName = new _char[iNameSize];
				ModelFile.read(pFileName, iNameSize);
				strcpy_s(szFullPath, szMatFilePath);
				strcat_s(szFullPath, pFileName);
				Safe_Delete_Array(pFileName);

				_tchar szTexturePath[MAX_PATH]{};
				MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTexturePath, MAX_PATH);

				Material.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szTexturePath);
				if (!Material.pMaterials[j])
				{
					MSG_BOX("Failed to Create Texture from Model!");
				}
			}

			m_Materials.push_back(Material);
		}

		ModelFile.close();
	}

	return S_OK;
}

HRESULT CModel::Init(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_Meshes[iMeshIndex]->Get_MatIndex();

	if (iMatIndex >= m_iNumMaterials)
	{
		return E_FAIL;
	}

	CTexture* pMaterial = m_Materials[iMatIndex].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return S_OK;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

CModel* CModel::Create(_dev pDevice, _context pContext, const string& strFilePath, _fmatrix OffsetMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, OffsetMatrix)))
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

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();
}
