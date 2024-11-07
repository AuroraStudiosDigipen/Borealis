/******************************************************************************
/*!
\file       SkinnedMeshImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 02, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <fstream>

#include "Importer/SkinnedMeshImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
	glm::mat4 ConvertMatrixtoGLM(aiMatrix4x4 const& mat)
	{
		glm::mat4 glm_mat;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		glm_mat[0][0] = mat.a1; glm_mat[1][0] = mat.a2; glm_mat[2][0] = mat.a3; glm_mat[3][0] = mat.a4;
		glm_mat[0][1] = mat.b1; glm_mat[1][1] = mat.b2; glm_mat[2][1] = mat.b3; glm_mat[3][1] = mat.b4;
		glm_mat[0][2] = mat.c1; glm_mat[1][2] = mat.c2; glm_mat[2][2] = mat.c3; glm_mat[3][2] = mat.c4;
		glm_mat[0][3] = mat.d1; glm_mat[1][3] = mat.d2; glm_mat[2][3] = mat.d3; glm_mat[3][3] = mat.d4;
		return glm_mat;
	}
}

namespace BorealisAssetCompiler
{
	void ExtractBoneWeight(std::vector<VertexBoneData>& vertices, aiMesh* mesh, const aiScene* scene, SkinnedModel& model)
	{
		auto& boneDataMap = model.mBoneDataMap;
		int& boneCount = model.mBoneCounter;

		for (unsigned int boneIndex{}; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneId = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneDataMap.find(boneName) == boneDataMap.end())
			{
				BoneData newBone;
				newBone.id = boneCount;
				newBone.offsetMatrix = ConvertMatrixtoGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneDataMap[boneName] = newBone;
				boneId = boneCount;
				++boneCount;
			}
			else
			{
				boneId = boneDataMap[boneName].id;
			}

			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex{}; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				vertices[vertexId].AddBoneData(boneId, weight);
			}
		}
	}

	SkinnedMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, SkinnedModel& model)
	{
		std::vector<glm::vec3> position;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<VertexBoneData> bones(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			position.push_back(vector);

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;

			normals.push_back(vector);

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				texCoords.push_back(vec);
			}
			else
				texCoords.push_back(glm::vec2(0.0f, 0.0f));

			bones[i].ResetBoneData();
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		ExtractBoneWeight(bones, mesh, scene, model);

		return SkinnedMesh(position, indices, normals, texCoords, bones);
	}

	void ProcessNode(aiNode* node, const aiScene* scene, SkinnedModel& model)
	{
		for (unsigned int i{}; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model.mMeshes.push_back(ProcessMesh(mesh, scene, model));
		}

		for (unsigned int i{}; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, model);
		}
	}

	void SkinnedMeshImporter::LoadFBXModel(SkinnedModel& model, const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return;
		}

		aiMaterial* material = scene->mMaterials[0];
		aiString texture_file;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
		if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {
			//returned pointer is not null, read texture from memory
		}
		else {
			//regular file, check if it exists and read it
		}

		ProcessNode(scene->mRootNode, scene, model);
	}

	void SkinnedMeshImporter::SaveSkinnedModel(SkinnedModel const& model, std::filesystem::path& cachePath)
	{
		std::ofstream outFile(cachePath, std::ios::binary);

		uint32_t meshCount = static_cast<uint32_t>(model.mMeshes.size());
		outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

		for (const SkinnedMesh& mesh : model.mMeshes) {
			// Write mesh header (vertexCount, indexCount, normalCount, texCoordCount)
			uint32_t verticesCount = mesh.mVerticesCount;
			uint32_t indicesCount = mesh.mIndicesCount;
			outFile.write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
			outFile.write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));

			// Write vertices
			outFile.write(reinterpret_cast<const char*>(mesh.mVertices.data()), verticesCount * sizeof(SkinnedVertex));

			// Write indices
			outFile.write(reinterpret_cast<const char*>(mesh.mIndices.data()), indicesCount * sizeof(uint32_t));
		}

		//save map of bonedata
		uint32_t boneDataMapSize = static_cast<uint32_t>(model.mBoneDataMap.size());
		outFile.write(reinterpret_cast<const char*>(&boneDataMapSize), sizeof(boneDataMapSize));

		for (const auto& [name, boneData] : model.mBoneDataMap) {
			// Write the length of the string key
			uint32_t nameLength = static_cast<uint32_t>(name.size());
			outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));

			// Write the string key
			outFile.write(name.data(), nameLength);

			// Write the BoneData
			outFile.write(reinterpret_cast<const char*>(&boneData.id), sizeof(boneData.id));
			outFile.write(reinterpret_cast<const char*>(&boneData.offsetMatrix), sizeof(boneData.offsetMatrix));
		}

		//save bone counter
		outFile.write(reinterpret_cast<const char*>(&model.mBoneCounter), sizeof(model.mBoneCounter));

		outFile.close();
	}
}

