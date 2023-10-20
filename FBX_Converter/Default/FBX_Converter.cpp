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

struct HyntraMesh
{
	std::string strName{};
	unsigned int iNumVertices{};
	unsigned int iNumFaces{};

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
					HyntraMesh MeshInfo{};
					MeshInfo.strName = pAIScene->mMeshes[i]->mName.C_Str();
					MeshInfo.iNumVertices = pAIScene->mMeshes[i]->mNumVertices;
					MeshInfo.iNumFaces = pAIScene->mMeshes[i]->mNumFaces;

					OutputFile.write(reinterpret_cast<const char*>(&MeshInfo), sizeof MeshInfo);

					Float3* vPos = new Float3[MeshInfo.iNumVertices];
					Float3* vNor = new Float3[MeshInfo.iNumVertices];
					Float2* vTex = new Float2[MeshInfo.iNumVertices];
					Float3* vTan = new Float3[MeshInfo.iNumVertices];
					for (size_t j = 0; j < MeshInfo.iNumVertices; j++)
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

					for (size_t j = 0; j < MeshInfo.iNumFaces; j++)
					{
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[0]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[1]), sizeof(unsigned int));
						OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mMeshes[i]->mFaces[j].mIndices[2]), sizeof(unsigned int));
					}
				}

				OutputFile.close();
			}
		}
	}
	std::cout << "Convert Success!" << std::endl;
	system("pause");
}