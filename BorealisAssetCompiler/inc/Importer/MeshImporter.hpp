/******************************************************************************
/*!
\file       MeshImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MeshImporter_HPP
#define MeshImporter_HPP

#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include "Importer/AssetConfigs.hpp"
#include "Importer/AssetStructs.hpp"

namespace BorealisAssetCompiler
{
	class MeshImporter
	{
	public:
		static void SaveFile(std::filesystem::path const& sourcePath, AssetConfig& assetConfig, std::filesystem::path& cachePath);

	private:
		static void LoadFBXModel(Model & model, const std::string& path);
		static void ConvertMeshToSOA(Mesh const& mesh, MeshSOA & meshSOA);
		static void SaveModel(Model const& model, std::filesystem::path& cachePath);
	};
}

#endif
