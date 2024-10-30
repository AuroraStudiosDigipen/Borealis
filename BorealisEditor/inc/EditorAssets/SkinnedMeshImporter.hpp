/******************************************************************************/
/*!
\file		SkinnedMeshImporter.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class for Skinned Mesh for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef SKINNEDMESHIMPORTER_HPP
#define SKINNEDMESHIMPORTER_HPP

#include <string>

#include <Core/core.hpp>
#include <Scene/Components.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Graphics/Model.hpp>
#include <Graphics/Animation/Bone.hpp>
#include <map>

#include "Graphics/SkinnedMesh.hpp"

namespace Borealis
{
	class SkinnedMeshImporter
	{
	public:
		static Ref<Model> LoadFBXModel(const std::string& path);
	private:
		static void ProcessNode(aiNode* node, const aiScene* scene, Model& model);
		static SkinnedMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& model);

		static void ExtractBoneWeight(std::vector<VertexBoneData>& vertices, aiMesh* mesh, const aiScene* scene, Model& model);
	};
}

#endif