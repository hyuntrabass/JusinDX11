#include "Custom_Part.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CCustom_Part::CCustom_Part(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CCustom_Part::CCustom_Part(const CCustom_Part& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
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

const _uint& CCustom_Part::Get_NumMeshes() const
{
	return m_iNumMeshes;
}

HRESULT CCustom_Part::Init_Prototype(ifstream& File, const string& strTexturePath, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Read_Meshes(File, PivotMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(Read_Materials(File, strTexturePath)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCustom_Part::Init(void* pArg)
{
	return S_OK;
}

HRESULT CCustom_Part::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pVariableName, vector<class CBone*>& Bones)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, Bones, pVariableName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CCustom_Part::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_Meshes[iMeshIndex]->Get_MatIndex();

	if (iMatIndex >= m_iNumMaterials)
	{
		return E_FAIL;
	}

	CTexture* pMaterial = m_Materials[iMatIndex].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}

HRESULT CCustom_Part::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CCustom_Part::Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos)
{
	for (auto& pMesh : m_Meshes)
	{
		if (pMesh->Intersect_RayMesh(WorldMatrix, pPickPos))
		{
			XMStoreFloat4(pPickPos, XMVector4Transform(XMLoadFloat4(pPickPos), WorldMatrix));
			return true;
		}
	}

	return false;
}

HRESULT CCustom_Part::Read_Meshes(ifstream& File, _fmatrix PivotMatrix)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, ModelType::Anim, File, PivotMatrix);
		if (!pMesh)
		{
			MSG_BOX("Failed to Read Meshes!");
			return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CCustom_Part::Read_Materials(ifstream& File, const string& strTexturePath)
{

	File.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		Model_Material Material{};
		_uint iNameSize{};
		_char* pFileName{};

		_char szFullPath[MAX_PATH]{};
		strcpy_s(szFullPath, strTexturePath.c_str());

		for (size_t j = 0; j < ToIndex(TextureType::End); j++)
		{
			File.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize == 1)
			{
				continue;
			}

			pFileName = new _char[iNameSize];
			File.read(pFileName, iNameSize);
			strcat_s(szFullPath, pFileName);
			Safe_Delete_Array(pFileName);

			_tchar szTexturePath[MAX_PATH]{};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTexturePath, MAX_PATH);

			Material.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szTexturePath);
			if (!Material.pMaterials[j])
			{
				MSG_BOX("Failed to Create Texture from Model!");
				return E_FAIL;
			}
		}

		m_Materials.push_back(Material);
	}

	return S_OK;
}

CCustom_Part* CCustom_Part::Create(_dev pDevice, _context pContext, ifstream& File, const string& strTexturePath, _fmatrix PivotMatrix)
{
	CCustom_Part* pInstance = new CCustom_Part(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(File, strTexturePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CCustom_Part");
	}

	return pInstance;
}

CComponent* CCustom_Part::Clone(void* pArg)
{
	CCustom_Part* pInstance = new CCustom_Part(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCustom_Part");
	}

	return pInstance;
}

void CCustom_Part::Free()
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
