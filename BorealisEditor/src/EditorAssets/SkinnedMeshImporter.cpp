/******************************************************************************/
/*!
\file		SkinnedMeshImporter.cpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	October 20, 2024
\brief		Declares the class Skinned Mesh Importer in Level Editor

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <EditorAssets/SkinnedMeshImporter.hpp>
#include <Graphics/Animation/VertexBone.hpp>
#include <map>

#include "EditorAssets/AnimationImporter.hpp"

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

namespace Borealis
{
	Ref<SkinnedModel> SkinnedMeshImporter::LoadFBXModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate /*| aiProcess_FlipUVs*/);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return nullptr;
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

		//BOREALIS_CORE_TRACE("FBX load from {0}", path);
		SkinnedModel model;
		ProcessNode(scene->mRootNode, scene, model);
		//model.mAnimation = AnimationImporter::LoadAnimations(path, MakeRef<SkinnedModel>(model));
		return MakeRef<SkinnedModel>(model);
	}

	SkinnedMesh SkinnedMeshImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, SkinnedModel& model)
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

	void SkinnedMeshImporter::ProcessNode(aiNode* node, const aiScene* scene, SkinnedModel& model)
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

	void SkinnedMeshImporter::ExtractBoneWeight(std::vector<VertexBoneData>& vertices, aiMesh* mesh, const aiScene* scene, SkinnedModel& model)
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

}
