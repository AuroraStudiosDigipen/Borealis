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
		Scene,
	};

	struct AssetMetaData
	{
		std::string name;

		AssetHandle Handle;
		AssetType Type;

		std::filesystem::path CachePath;
		std::filesystem::path SourcePath;

		std::time_t importDate;
		std::time_t cacheCreationDate;
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
		static void SaveMetaFile(AssetMetaData metaData, std::filesystem::path const& metaFilePath);
	};
}

#endif
