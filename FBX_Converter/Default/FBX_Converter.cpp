#include <filesystem>
#include <iostream>
#include <fstream>
#include <stack>
#include <Windows.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else //Release
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

struct Uint4
{
	unsigned int x{};
	unsigned int y{};
	unsigned int z{};
	unsigned int w{};
};

struct Float4
{
	float x{};
	float y{};
	float z{};
	float w{};
};

int main()
{
	unsigned int iNumStaticFiles{};
	unsigned int iNumAnimFiles{};
	Assimp::Importer Importer{};
	const aiScene* pAIScene{ nullptr };
	int iNumMesh{};

	std::string InputFilePath = "../StaticMesh/";
	std::cout << "Static Models : Start Converting..." << std::endl;

#pragma region Static Meshes
	//for (const auto& entry : std::filesystem::recursive_directory_iterator(InputFilePath))
	//{
	//	if (entry.is_regular_file())
	//	{
	//		if (entry.path().extension() != ".fbx")
	//		{
	//			continue;
	//		}

	//		std::cout << "Converting... " << entry.path().filename().string() << std::endl;

	//		std::filesystem::path relative_path = std::filesystem::relative(entry.path().parent_path(), InputFilePath);
	//		std::filesystem::path OutputFilePath = std::filesystem::path("../Output/Static/") / relative_path;
	//		if (!std::filesystem::exists(OutputFilePath))
	//		{
	//			std::filesystem::create_directories(OutputFilePath);
	//		}
	//		OutputFilePath /= entry.path().stem().string() + ".hyuntrastatmesh";
	//		std::ofstream OutputFile(OutputFilePath.c_str(), std::ios::binary);

	//		if (OutputFile.is_open())
	//		{
	//			pAIScene = Importer.ReadFile(entry.path().string(), aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
	//			if (pAIScene)
	//			{
	//				std::cout << "Read File Succssefully" << std::endl;
	//			}

	//		#pragma region Meshes
	//			OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mNumMeshes), sizeof(unsigned int));

	//			for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
	//			{
	//				aiMesh* pMesh = pAIScene->mMeshes[i];

	//				unsigned int iMatIndex = pMesh->mMaterialIndex;
	//				std::string strName = pMesh->mName.C_Str();
	//				unsigned int iNameSize = strName.size() + 1;
	//				unsigned int iNumVertices = pMesh->mNumVertices;
	//				unsigned int iNumFaces = pMesh->mNumFaces;

	//				OutputFile.write(reinterpret_cast<const char*>(&iMatIndex), sizeof(unsigned int));
	//				OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
	//				OutputFile.write(strName.c_str(), iNameSize);
	//				OutputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(unsigned int));
	//				OutputFile.write(reinterpret_cast<const char*>(&iNumFaces), sizeof(unsigned int));

	//				for (size_t j = 0; j < iNumVertices; j++)
	//				{
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mVertices[j]), sizeof aiVector3D);
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mNormals[j]), sizeof aiVector3D);
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTextureCoords[0][j]), sizeof aiVector2D);
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTangents[j]), sizeof aiVector3D);
	//				}

	//				for (size_t j = 0; j < iNumFaces; j++)
	//				{
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[0]), sizeof(unsigned int));
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[1]), sizeof(unsigned int));
	//					OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[2]), sizeof(unsigned int));
	//				}
	//			}
	//		#pragma endregion

	//		#pragma region Materials
	//			unsigned int NumMaterials = pAIScene->mNumMaterials;
	//			OutputFile.write(reinterpret_cast<const char*>(&NumMaterials), sizeof(unsigned int));

	//			for (size_t i = 0; i < NumMaterials; i++)
	//			{
	//				aiMaterial* pAIMaterial = pAIScene->mMaterials[i];
	//				unsigned int iTexturePathNameSize{};

	//				for (size_t j = 0; j < 18; j++)
	//				{
	//					aiString strTexturePath{};
	//					pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath);
	//					char TextureFileName[_MAX_PATH]{};
	//					char TextureExt[_MAX_PATH]{};
	//					_splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, TextureFileName, _MAX_PATH, TextureExt, _MAX_PATH);

	//					strcat_s(TextureFileName, TextureExt);
	//					iTexturePathNameSize = strlen(TextureFileName) + 1;
	//					OutputFile.write(reinterpret_cast<const char*>(&iTexturePathNameSize), sizeof(unsigned int));
	//					if (iTexturePathNameSize == 1)
	//					{
	//						continue;
	//					}
	//					OutputFile.write(TextureFileName, iTexturePathNameSize);
	//				}
	//			}
	//		#pragma endregion

	//			iNumStaticFiles++;
	//			OutputFile.close();
	//		}
	//	}
	//}
#pragma endregion


	std::cout << "Static Models : Convert Success!\n" << std::endl;

	std::cout << "Animation Models : Start Converting..." << std::endl;

#pragma region Animation Meshes
	InputFilePath = "../AnimMesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(InputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() != ".fbx")
			{
				continue;
			}
			std::cout << "Converting... " << entry.path().filename().string() << std::endl;

			// InputFilePath�� ���������� ����θ� ���Ѵ�.
			std::filesystem::path relative_path = std::filesystem::relative(entry.path().parent_path(), InputFilePath);

			// ������(/)�� Output��ο� ������ ���� ��θ� ��ģ��.
			std::filesystem::path OutputFilePath = std::filesystem::path("../Output/Anim/") / relative_path;

			// ������ ������ �����Ѵ�.
			if (!std::filesystem::exists(OutputFilePath))
			{
				std::filesystem::create_directories(OutputFilePath);
			}

			// ���� ���� ��θ� ����
			OutputFilePath /= entry.path().stem().string() + ".hyuntraanimmesh";
			std::ofstream OutputFile(OutputFilePath.c_str(), std::ios::binary);

			if (OutputFile.is_open())
			{
				pAIScene = Importer.ReadFile(entry.path().string(), aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
				if (pAIScene)
				{
					std::cout << "Read File Succssefully" << std::endl;
				}
				std::vector<aiString> BoneNames{};
			#pragma region Bones
				std::stack<aiNode*> Nodes{};
				Nodes.push(pAIScene->mRootNode);
				unsigned int iNumBones{};
				while (!Nodes.empty())
				{
					aiNode* pCurrNode = Nodes.top();
					Nodes.pop();
					iNumBones++;

					for (size_t i = 0; i < pCurrNode->mNumChildren; i++)
					{
						Nodes.push(pCurrNode->mChildren[i]);
					}
				}
				OutputFile.write(reinterpret_cast<const char*>(&iNumBones), sizeof(unsigned int));

				std::list<std::pair<aiNode*, int>> Bones{};
				int iMyIndex{ -1 };
				Bones.push_front({ pAIScene->mRootNode, -1 });

				while (!Bones.empty())
				{
					aiNode* pCurrentBone = Bones.front().first;
					int iParentIndex = Bones.front().second;
					Bones.pop_front();

					// �ʿ��� �� ������ ���.
					char szName[_MAX_PATH]{};
					aiString BoneName = pCurrentBone->mName;
					BoneNames.push_back(BoneName);

					strcpy_s(szName, BoneName.C_Str());
					unsigned int iNameSize = strlen(szName) + 1;
					OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
					OutputFile.write(BoneName.C_Str(), iNameSize);
					OutputFile.write(reinterpret_cast<const char*>(&pCurrentBone->mTransformation.Transpose()), sizeof(aiMatrix4x4));
					OutputFile.write(reinterpret_cast<const char*>(&iParentIndex), sizeof(int));

					iMyIndex++;

					for (size_t i = 0; i < pCurrentBone->mNumChildren; i++)
					{
						Bones.push_back({ pCurrentBone->mChildren[i], iMyIndex });
					}
				}
			#pragma endregion

			#pragma region Meshes
				OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mNumMeshes), sizeof(unsigned int));

				for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
				{
					aiMesh* pMesh = pAIScene->mMeshes[i];

					unsigned int iMatIndex = pMesh->mMaterialIndex;
					aiString strName = pMesh->mName;
					unsigned int iNameSize = strName.length + 1;
					unsigned int iNumVertices = pMesh->mNumVertices;
					unsigned int iNumFaces = pMesh->mNumFaces;
					unsigned int iNumBones = pMesh->mNumBones;

					OutputFile.write(reinterpret_cast<const char*>(&iMatIndex), sizeof(unsigned int));
					OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
					OutputFile.write(strName.C_Str(), iNameSize);
					OutputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(unsigned int));
					OutputFile.write(reinterpret_cast<const char*>(&iNumFaces), sizeof(unsigned int));
					OutputFile.write(reinterpret_cast<const char*>(&iNumBones), sizeof(unsigned int));

					Uint4* vBlendIndices = new Uint4[iNumVertices]{};
					Float4* vBlendWeights = new Float4[iNumVertices]{};

					for (size_t j = 0; j < iNumBones; j++)
					{
						aiBone* pBone = pMesh->mBones[j];

						for (size_t k = 0; k < pBone->mNumWeights; k++)
						{
							if (vBlendWeights[pBone->mWeights[k].mVertexId].x == 0.f)
							{
								vBlendIndices[pBone->mWeights[k].mVertexId].x = j;
								vBlendWeights[pBone->mWeights[k].mVertexId].x = pBone->mWeights[k].mWeight;
							}
							else if (vBlendWeights[pBone->mWeights[k].mVertexId].y == 0.f)
							{
								vBlendIndices[pBone->mWeights[k].mVertexId].y = j;
								vBlendWeights[pBone->mWeights[k].mVertexId].y = pBone->mWeights[k].mWeight;
							}
							else if (vBlendWeights[pBone->mWeights[k].mVertexId].z == 0.f)
							{
								vBlendIndices[pBone->mWeights[k].mVertexId].z = j;
								vBlendWeights[pBone->mWeights[k].mVertexId].z = pBone->mWeights[k].mWeight;
							}
							else if (vBlendWeights[pBone->mWeights[k].mVertexId].w == 0.f)
							{
								vBlendIndices[pBone->mWeights[k].mVertexId].w = j;
								vBlendWeights[pBone->mWeights[k].mVertexId].w = pBone->mWeights[k].mWeight;
							}
						}
					}

					for (size_t j = 0; j < iNumVertices; j++)
					{
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mVertices[j]), sizeof aiVector3D);
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mNormals[j]), sizeof aiVector3D);
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTextureCoords[0][j]), sizeof aiVector2D);
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTangents[j]), sizeof aiVector3D);
						OutputFile.write(reinterpret_cast<const char*>(&vBlendIndices[j]), sizeof Uint4);
						OutputFile.write(reinterpret_cast<const char*>(&vBlendWeights[j]), sizeof Float4);
					}

					delete[] vBlendIndices;
					delete[] vBlendWeights;

					for (size_t j = 0; j < iNumBones; j++)
					{
						aiBone* pBone = pMesh->mBones[j];

						OutputFile.write(reinterpret_cast<const char*>(&pBone->mOffsetMatrix.Transpose()), sizeof aiMatrix4x4);

						unsigned int iBoneIndex{};
						std::find_if(BoneNames.begin(), BoneNames.end(), [&pBone, &iBoneIndex](aiString strBoneName)
						{
							if (pBone->mName == strBoneName)
							{
								return true;
							}
							iBoneIndex++;
							return false;
						});

						OutputFile.write(reinterpret_cast<const char*>(&iBoneIndex), sizeof(unsigned int));
					}

					for (size_t j = 0; j < iNumFaces; j++)
					{
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[0]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[1]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[2]), sizeof(unsigned int));
					}
				}
			#pragma endregion

			#pragma region Materials
				unsigned int NumMaterials = pAIScene->mNumMaterials;
				OutputFile.write(reinterpret_cast<const char*>(&NumMaterials), sizeof(unsigned int));

				for (size_t i = 0; i < NumMaterials; i++)
				{
					aiMaterial* pAIMaterial = pAIScene->mMaterials[i];
					unsigned int iTexturePathNameSize{};

					for (size_t j = 0; j < 18; j++)
					{
						aiString strTexturePath{};
						pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath);
						char TextureFileName[_MAX_PATH]{};
						char TextureExt[_MAX_PATH]{};
						_splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, TextureFileName, _MAX_PATH, TextureExt, _MAX_PATH);

						strcat_s(TextureFileName, TextureExt);
						iTexturePathNameSize = strlen(TextureFileName) + 1;
						OutputFile.write(reinterpret_cast<const char*>(&iTexturePathNameSize), sizeof(unsigned int));
						if (iTexturePathNameSize == 1)
						{
							continue;
						}
						OutputFile.write(TextureFileName, iTexturePathNameSize);
					}
				}
			#pragma endregion

			#pragma region Animations
				unsigned int iNumAnimations = pAIScene->mNumAnimations;
				OutputFile.write(reinterpret_cast<const char*>(&iNumAnimations), sizeof(unsigned int));

				for (size_t i = 0; i < iNumAnimations; i++)
				{
					aiAnimation* pAnimation = pAIScene->mAnimations[i];

					unsigned int iNameSize = pAnimation->mName.length + 1;
					OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
					OutputFile.write(pAnimation->mName.C_Str(), iNameSize);

					float fDuration{ static_cast<float>(pAnimation->mDuration) };
					float fTickPerSec{ static_cast<float>(pAnimation->mTicksPerSecond) };
					OutputFile.write(reinterpret_cast<const char*>(&fDuration), sizeof(float));
					OutputFile.write(reinterpret_cast<const char*>(&fTickPerSec), sizeof(float));

					unsigned int iNumChannels = pAnimation->mNumChannels;
					OutputFile.write(reinterpret_cast<const char*>(&iNumChannels), sizeof(unsigned int));

					for (size_t j = 0; j < iNumChannels; j++)
					{
						aiNodeAnim* pChannel = pAnimation->mChannels[j];

						unsigned int iChannelNameSize = pChannel->mNodeName.length + 1;
						OutputFile.write(reinterpret_cast<const char*>(&iChannelNameSize), sizeof(unsigned int));
						OutputFile.write(pChannel->mNodeName.C_Str(), iChannelNameSize);

						unsigned int iNumKeyFrame = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
						iNumKeyFrame = max(iNumKeyFrame, pChannel->mNumPositionKeys);
						OutputFile.write(reinterpret_cast<const char*>(&iNumKeyFrame), sizeof(unsigned int));

						float fTime{};
						float fScaleW{0.f};
						float fPositionW{1.f};

						for (size_t k = 0; k < iNumKeyFrame; k++)
						{
							if (k < pChannel->mNumScalingKeys)
							{
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mScalingKeys->mValue), sizeof(aiVector3D));
								OutputFile.write(reinterpret_cast<const char*>(&fScaleW), sizeof(float));

								fTime = static_cast<float>(pChannel->mScalingKeys->mTime);
							}
							if (k < pChannel->mNumRotationKeys)
							{
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys->mValue.x), sizeof(float));
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys->mValue.y), sizeof(float));
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys->mValue.z), sizeof(float));
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys->mValue.w), sizeof(float));

								fTime = static_cast<float>(pChannel->mRotationKeys->mTime);
							}
							if (k < pChannel->mNumPositionKeys)
							{
								OutputFile.write(reinterpret_cast<const char*>(&pChannel->mPositionKeys->mValue), sizeof(aiVector3D));
								OutputFile.write(reinterpret_cast<const char*>(&fPositionW), sizeof(float));

								fTime = static_cast<float>(pChannel->mPositionKeys->mTime);
							}

							OutputFile.write(reinterpret_cast<const char*>(&fTime), sizeof(float));
						}
					}
				}
			#pragma endregion

				iNumAnimFiles++;
				OutputFile.close();
			}
		}
	}
#pragma endregion

	std::cout << "Animation Models : Convert Success!\n" << std::endl;

	std::cout << iNumStaticFiles << " Static Models & " << iNumAnimFiles << " Animation Models are Successfully Converted as hyuntramesh!!" << std::endl;
	system("pause");
}