#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else //Release
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

struct Float3
{
	float x{};
	float y{};
	float z{};
};

struct Float2
{
	float x{};
	float y{};
};

int main()
{
	Assimp::Importer Importer{};
	const aiScene* pAIScene{ nullptr };
	int iNumMesh{};

	std::string InputFilePath = "../StaticMesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(InputFilePath))
	{
		if (entry.is_regular_file())
		{
			std::cout << "Converting... " << entry.path().filename().string() << std::endl;
			std::string OutputFilePath = "../Output/";
			OutputFilePath += entry.path().stem().string() + ".hyntrastatmesh";
			std::ofstream OutputFile(OutputFilePath.c_str(), std::ios::binary);

			if (OutputFile.is_open())
			{
				pAIScene = Importer.ReadFile(entry.path().string(), aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

				OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mNumMeshes), sizeof(unsigned int));

				for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
				{
					unsigned int iMatIndex = pAIScene->mMeshes[i]->mMaterialIndex;
					std::string strName = pAIScene->mMeshes[i]->mName.C_Str();
					unsigned int iNameSize = strName.size() + 1;
					unsigned int iNumVertices = pAIScene->mMeshes[i]->mNumVertices;
					unsigned int iNumFaces = pAIScene->mMeshes[i]->mNumFaces;

					OutputFile.write(reinterpret_cast<const char*>(&iMatIndex), sizeof(unsigned int));
					OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
					OutputFile.write(strName.c_str(), iNameSize);
					OutputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(unsigned int));
					OutputFile.write(reinterpret_cast<const char*>(&iNumFaces), sizeof(unsigned int));

					Float3* vPos = new Float3[iNumVertices];
					Float3* vNor = new Float3[iNumVertices];
					Float2* vTex = new Float2[iNumVertices];
					Float3* vTan = new Float3[iNumVertices];
					for (size_t j = 0; j < iNumVertices; j++)
					{
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mVertices[j]), sizeof Float3);
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mNormals[j]), sizeof Float3);
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mTextureCoords[0][j]), sizeof Float2);
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mTangents[j]), sizeof Float3);
					}

					delete[] vPos;
					delete[] vNor;
					delete[] vTex;
					delete[] vTan;

					for (size_t j = 0; j < iNumFaces; j++)
					{
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[0]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[1]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[2]), sizeof(unsigned int));
					}
				}

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
						unsigned int strleng = strTexturePath.length;
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

				OutputFile.close();
			}
		}
	}
	std::cout << "Convert Success!" << std::endl;
	system("pause");
}