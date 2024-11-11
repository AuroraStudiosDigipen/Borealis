/******************************************************************************
/*!
\file       MetaSerializer.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 23, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MetaSerializer_HPP
#define MetaSerializer_HPP

#include <Assets/AssetMetaData.hpp>

#define META_VERSION 0.01

namespace Borealis
{
	class MetaFileSerializer
	{
	public:
		/*!***********************************************************************
			\brief
				Set assets folder path
		*************************************************************************/
		static void SetAssetFolderPath(std::filesystem::path const& path);

		/*!***********************************************************************
			\brief
				get meta data from file path
		*************************************************************************/
		static AssetMetaData GetAssetMetaDataFile(std::filesystem::path const& path);

		/*!***********************************************************************
			\brief
				create meta data at file path
		*************************************************************************/
		static AssetMetaData CreateAssetMetaFile(std::filesystem::path const& path);

		/*!***********************************************************************
			\brief
				create meta data at file path with given asset handle
		*************************************************************************/
		static AssetMetaData CreateAssetMetaFile(std::filesystem::path const& path, AssetHandle handle);

		/*!***********************************************************************
			\brief
				serialize registry
		*************************************************************************/
		static void SerialzeRegistry(std::filesystem::path const& assetRegistryPath, std::unordered_map<AssetHandle, AssetMetaData> const& assetRegistry);

		/*!***********************************************************************
			\brief
				deserialize registry
		*************************************************************************/
		static void DeserializeRegistry(std::string const& registryFileString, std::unordered_map<AssetHandle, AssetMetaData>& AssetRegistry);

		static uint32_t HashFile(std::filesystem::path const& path);

	private:
		static void SaveAsFile(const std::filesystem::path& path, const char* outputFile);
		static std::time_t GetLastWriteTime(const std::filesystem::path& path);
		static AssetMetaData GetAssetMetaData(std::filesystem::path const& path);
		static AssetConfig GetDefaultConfig(AssetType type);

	private:
		inline static std::filesystem::path PathToAssetFolder;
	};
}

#endif
