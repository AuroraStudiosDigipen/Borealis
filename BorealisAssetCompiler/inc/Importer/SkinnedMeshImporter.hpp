/******************************************************************************
/*!
\file       SkinnedMeshImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SkinnedMeshImporter_HPP
#define SkinnedMeshImporter_HPP

#include <vector>
#include <filesystem>
#include <string>

#include "Importer/AssetStructs.hpp"

namespace BorealisAssetCompiler
{
	class SkinnedMeshImporter
	{
	public:
		static void LoadFBXModel(SkinnedModel& model, const std::string& path);
		static void SaveSkinnedModel(SkinnedModel const& model, std::filesystem::path& cachePath);
	private:
		//static void ProcessNode(aiNode* node, const aiScene* scene, SkinnedModel& model);
		//static SkinnedMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, SkinnedModel& model);

		//static void ExtractBoneWeight(std::vector<VertexBoneData>& vertices, aiMesh* mesh, const aiScene* scene, SkinnedModel& model);
	};
}

#endif
