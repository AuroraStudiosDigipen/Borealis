/******************************************************************************/
/*!
\file		MeshImporter.cpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 12, 2024
\brief		Declares the class Mesh Importer in Level Editor

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <EditorAssets/MeshImporter.hpp>
#include <Graphics/Animation/VertexBone.hpp>
#include <map>

namespace Borealis
{
	Ref<Model> MeshImporter::LoadFBXModel(const std::string& path)
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
		Model model;
		ProcessNode(scene->mRootNode, scene, model);
		return MakeRef<Model>(model);
	}

	Mesh MeshImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertices.push_back(vector);

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
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Mesh(vertices, indices, normals, texCoords);
	}

	void MeshImporter::ProcessNode(aiNode* node, const aiScene* scene, Model& model)
	{
		for (unsigned int i{}; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model.mMeshes.push_back(ProcessMesh(mesh, scene));
		}

		for (unsigned int i{}; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, model);
		}
	}
}