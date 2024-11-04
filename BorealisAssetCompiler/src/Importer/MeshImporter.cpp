/******************************************************************************
/*!
\file       MeshImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Importer/AnimationImporter.hpp"
#include "Importer/MeshImporter.hpp"
#include "Importer/SkinnedMeshImporter.hpp"
#include "Importer/MeshOptimizer.hpp"


namespace BorealisAssetCompiler
{
	void MeshImporter::SaveFile(std::filesystem::path const& sourcePath, AssetConfig& assetConfig, std::filesystem::path& cachePath)
	{
		MeshConfig config = GetConfig<MeshConfig>(assetConfig);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(sourcePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (scene->HasAnimations() || scene->hasSkeletons())
		{
			config.skinMesh = true;
			SkinnedModel skinnedModel;

			SkinnedMeshImporter::LoadFBXModel(skinnedModel, sourcePath.string());

			Animation anim;
			cachePath.replace_extension(".anim");
			AnimationImporter::LoadAnimations(anim, sourcePath.string());
			AnimationImporter::SaveAnimation(anim, cachePath);

			cachePath.replace_extension(".skmesh");
			SkinnedMeshImporter::SaveSkinnedModel(skinnedModel, cachePath);


		}
		else
		{
			config.skinMesh = false;
			Model model;

			LoadFBXModel(model, sourcePath.string());

			OptimizeModel(model);

			cachePath.replace_extension(".mesh");
			SaveModel(model, cachePath);
		}
		assetConfig = config;
	}

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		Mesh retMesh;

		std::vector<Vertex> &vertices = retMesh.mVertices;

		std::vector<unsigned int> & indices = retMesh.mIndices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;

			vertex.Normal = vector;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		retMesh.mIndicesCount = (uint32_t)retMesh.mIndices.size();
		retMesh.mVerticesCount = (uint32_t)retMesh.mVertices.size();

		return retMesh;
	}

	void ProcessNode(aiNode* node, const aiScene* scene, Model& model)
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

	void MeshImporter::LoadFBXModel(Model& model, const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return;
		}

		ProcessNode(scene->mRootNode, scene, model);
	}

	void MeshImporter::SaveModel(Model const& model, std::filesystem::path& cachePath)
	{
		std::ofstream outFile(cachePath, std::ios::binary);

		uint32_t meshCount = static_cast<uint32_t>(model.mMeshes.size());
		outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

		for (const Mesh& mesh : model.mMeshes) {
			uint32_t verticesCount = mesh.mVerticesCount;
			uint32_t indicesCount = mesh.mIndicesCount;
			outFile.write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
			outFile.write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));

			outFile.write(reinterpret_cast<const char*>(mesh.mVertices.data()), verticesCount * sizeof(Vertex));

			outFile.write(reinterpret_cast<const char*>(mesh.mIndices.data()), indicesCount * sizeof(uint32_t));
		}

		outFile.close();
	}
}

