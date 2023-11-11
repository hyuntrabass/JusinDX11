#include "Custom_Model.h"
#include "Bone.h"
#include "Animation.h"
#include "Custom_Part.h"

CCustom_Model::CCustom_Model(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CCustom_Model::CCustom_Model(const CCustom_Model& rhs)
	: CComponent(rhs)
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

	for (size_t i = 0; i < PT_END; i++)
	{
		m_Parts[i] = rhs.m_Parts[i];
		m_iNumParts[i] = rhs.m_iNumParts[i];
		for (auto& pPart : m_Parts[i])
		{
			Safe_AddRef(pPart);
		}
	}
}

const _uint& CCustom_Model::Get_NumMeshes(_uint eType) const
{
	return m_Parts[eType][m_iSelectedPartIndex[eType]]->Get_NumMeshes();
}

const _bool& CCustom_Model::IsAnimationFinished(_uint iAnimIndex) const
{
	return m_Animations[iAnimIndex]->IsFinished();
}

const _uint& CCustom_Model::Get_CurrentAnimationIndex() const
{
	return m_iCurrentAnimIndex;
}

void CCustom_Model::Set_Animation(_uint iAnimIndex, const _bool& isLoop)
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

void CCustom_Model::Select_Part(_uint eType, _uint iPartIndex)
{
	if (iPartIndex >= m_iNumParts[eType])
	{
		m_iSelectedPartIndex[eType] = m_iNumParts[eType] - 1;
		return;
	}
	m_iSelectedPartIndex[eType] = iPartIndex;
}

HRESULT CCustom_Model::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_char szMatFilePath[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
	string strTexturePath = szMatFilePath;
	strTexturePath += "../Texture/";

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (FAILED(Read_Bones(ModelFile)))
		{
			return E_FAIL;
		}

		if (FAILED(Read_Animations(ModelFile)))
		{
			return E_FAIL;
		}

		for (size_t i = 0; i < PT_END; i++)
		{
			ModelFile.read(reinterpret_cast<_char*>(&m_iNumParts[i]), sizeof _uint);

			for (size_t j = 0; j < m_iNumParts[i]; j++)
			{
				CCustom_Part* pPart = CCustom_Part::Create(m_pDevice, m_pContext, ModelFile, strTexturePath, PivotMatrix);
				if (!pPart)
				{
					return E_FAIL;
				}
				m_Parts[i].push_back(pPart);
			}
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

HRESULT CCustom_Model::Init(void* pArg)
{
	return S_OK;
}

void CCustom_Model::Play_Animation(_float fTimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta, m_isAnimChanged, m_isLoop);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedMatrix(m_Bones);
	}
}

HRESULT CCustom_Model::Bind_BoneMatrices(_uint eType, _uint iMeshIndex, CShader* pShader, const _char* pVariableName)
{
	return m_Parts[eType][m_iSelectedPartIndex[eType]]->Bind_BoneMatrices(iMeshIndex, pShader, pVariableName, m_Bones);
}

HRESULT CCustom_Model::Bind_Material(_uint eType, CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	if (FAILED(m_Parts[eType][m_iSelectedPartIndex[eType]]->Bind_Material(pShader, pVariableName, iMeshIndex, eTextureType)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCustom_Model::Render(_uint eType, _uint iMeshIndex)
{
	return m_Parts[eType][m_iSelectedPartIndex[eType]]->Render(iMeshIndex);
}

HRESULT CCustom_Model::Read_Bones(ifstream& File)
{
	_uint iNumBones{};
	File.read(reinterpret_cast<_char*>(&iNumBones), sizeof _uint);

	for (size_t i = 0; i < iNumBones; i++)
	{
		CBone* pBone = CBone::Create(File);
		if (!pBone)
		{
			MSG_BOX("Failed to Read Bone!");
			return E_FAIL;
		}
		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CCustom_Model::Read_Animations(ifstream& File)
{
	File.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(File);
		if (!pAnimation)
		{
			MSG_BOX("Failed to Read Animations!");
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CCustom_Model* CCustom_Model::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CCustom_Model* pInstance = new CCustom_Model(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CCustom_Model");
	}

	return pInstance;
}

CComponent* CCustom_Model::Clone(void* pArg)
{
	CCustom_Model* pInstance = new CCustom_Model(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCustom_Model");
	}

	return pInstance;
}

void CCustom_Model::Free()
{
	__super::Free();

	for (size_t i = 0; i < PT_END; i++)
	{
		for (auto& pPart : m_Parts[i])
		{
			Safe_Release(pPart);
		}
		m_Parts->clear();
	}

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
}
