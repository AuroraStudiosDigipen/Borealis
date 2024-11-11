/******************************************************************************
/*!
\file       MetaSerializer.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MetaSerializer_HPP
#define MetaSerializer_HPP

#include <string>
#include <filesystem>

#include "Importer/AssetConfigs.hpp"

namespace BorealisAssetCompiler
{
	using AssetHandle = uint64_t;

	enum class AssetType
	{
		None,
		Audio,
		Mesh,
		Shader,
		Texture2D,
		Folder,
		Font,
		Scene,
		Material,
		Prefab
	};

	struct AssetMetaData
	{
		double Version;
		std::string name;

		AssetHandle Handle;
		AssetType Type;

		AssetConfig Config;

		std::filesystem::path CachePath;
		std::filesystem::path SourcePath;

		uint32_t SourceFileHash = 0;
	};

	class Asset
	{
	public:
		static std::string AssetTypeToString(AssetType type);
		static AssetType StringToAssetType(std::string const& type);
	};

	class MetaSerializer
	{
	public:
		static AssetMetaData GetAssetMetaDataFile(std::filesystem::path const& path);
		static AssetConfig GetDefaultConfig(AssetType type);
		static void SaveMetaFile(AssetMetaData metaData, std::filesystem::path const& metaFilePath);
	};
}

#endif
