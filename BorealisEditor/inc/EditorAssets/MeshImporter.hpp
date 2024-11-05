/******************************************************************************/
/*!
\file		MeshImporter.hpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 12, 2024
\brief		Declares the class Mesh Importer in Level Editor

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef MESH_IMPORTER_HPP
#define MESH_IMPORTER_HPP

#include <string>

#include <Core/core.hpp>
#include <Scene/Components.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Graphics/Model.hpp>
#include <map>

namespace Borealis
{
	class MeshImporter
	{
	public:
		static Ref<Model> LoadFBXModel(const std::string& path);
	private:
		static void ProcessNode(aiNode* node, const aiScene* scene, Model& model);
		static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}

#endif