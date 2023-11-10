#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

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
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
{
	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		CBone* pBone = pPrototypeBone->Clone();

		m_Bones.push_back(pBone);
	}

	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		CAnimation* pAnimation = pPrototypeAnimation->Clone();

		m_Animations.push_back(pAnimation);
	}

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

const _bool& CModel::IsAnimationFinished(_uint iAnimIndex) const
{
	return m_Animations[iAnimIndex]->IsFinished();
}

const _uint& CModel::Get_CurrentAnimationIndex() const
{
	return m_iCurrentAnimIndex;
}

void CModel::Set_Animation(_uint iAnimIndex, const _bool& isLoop)
{
	m_isLoop = isLoop;
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		m_isAnimChanged = true;

		for (auto& pAnim : m_Animations)
		{
			pAnim->ResetFinished();
		}
	}
	m_iCurrentAnimIndex = iAnimIndex;
}

HRESULT CModel::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	ModelType eType{};
	_char szExt[MAX_PATH]{};
	if (!isCOLMesh)
	{
		_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
		if (!strcmp(szExt, ".hyuntraanimmesh"))
		{
			eType = ModelType::Anim;
		}
		else if (!strcmp(szExt, ".hyuntraplayer"))
		{
			eType = ModelType::Player;
		}
		else
		{
			eType = ModelType::Static;
		}
	}
	else
	{
		eType = ModelType::Collision;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (eType == ModelType::Player)
		{
		#pragma region Bones
			_uint iNumBones{};
			ModelFile.read(reinterpret_cast<_char*>(&iNumBones), sizeof _uint);

			for (size_t i = 0; i < iNumBones; i++)
			{
				CBone* pBone = CBone::Create(ModelFile);
				if (!pBone)
				{
					return E_FAIL;
				}
				m_Bones.push_back(pBone);
			}
		#pragma endregion

		#pragma region Animations
			ModelFile.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

			for (size_t i = 0; i < m_iNumAnimations; i++)
			{
				CAnimation* pAnimation = CAnimation::Create(ModelFile);
				if (!pAnimation)
				{
					return E_FAIL;
				}
				m_Animations.push_back(pAnimation);
			}
		#pragma endregion



			return S_OK;
		}

		if (eType == ModelType::Anim)
		{
		#pragma region Bones
			_uint iNumBones{};
			ModelFile.read(reinterpret_cast<_char*>(&iNumBones), sizeof _uint);

			for (size_t i = 0; i < iNumBones; i++)
			{
				CBone* pBone = CBone::Create(ModelFile);
				if (!pBone)
				{
					return E_FAIL;
				}
				m_Bones.push_back(pBone);
			}
		#pragma endregion
		}

	#pragma region Meshes
		ModelFile.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
		m_Meshes.reserve(m_iNumMeshes);

		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, ModelFile, PivotMatrix);
			if (!pMesh)
			{
				return E_FAIL;
			}
			m_Meshes.push_back(pMesh);
		}
	#pragma endregion

	#pragma region Materials
		_char szMatFilePath[MAX_PATH]{};
		_char szFullPath[MAX_PATH]{};

		_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
		strcat_s(szMatFilePath, "../Texture/");

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
	#pragma endregion

		if (eType == ModelType::Anim)
		{
		#pragma region Animations
			ModelFile.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

			for (size_t i = 0; i < m_iNumAnimations; i++)
			{
				CAnimation* pAnimation = CAnimation::Create(ModelFile);
				if (!pAnimation)
				{
					return E_FAIL;
				}
				m_Animations.push_back(pAnimation);
			}
		#pragma endregion
		}


		ModelFile.close();
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Init(void* pArg)
{
	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta, m_isAnimChanged, m_isLoop);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedMatrix(m_Bones);
	}
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pVariableName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pVariableName, XMLoadFloat4x4(&m_PivotMatrix));
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
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos)
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

CModel* CModel::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
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

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& pAinmation : m_Animations)
	{
		Safe_Release(pAinmation);
	}
	m_Animations.clear();

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
