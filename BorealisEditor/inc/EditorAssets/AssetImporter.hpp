/******************************************************************************
/*!
\file       AssetImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetImporter_HPP
#define AssetImporter_HPP

#include <Core/ProjectInfo.hpp>

#include <string>
#include <filesystem>

#include <Assets/EditorAssetManager.hpp>

namespace Borealis
{
	class AssetImporter
	{
	public:
		void LoadRegistry(Borealis::ProjectInfo projectInfo);

		static AssetHandle GetAssetHandle(std::filesystem::path const& path);

	private:
		bool ImportAsset(AssetMetaData metaData);

		void SerializeRegistry();
		void DeserializeRegistry(std::string const& registryFileString, AssetRegistry& assetRegistry);

		void RegisterAsset(std::filesystem::path path, AssetRegistry& assetRegistry);
		void RegisterAllAssets(std::filesystem::path path, AssetRegistry& assetRegistry);

		bool VerifyMetaFile(std::filesystem::path path, AssetRegistry& assetRegistry);


	private:
		std::filesystem::path mAssetRegistryPath;
		inline static std::unordered_map<std::size_t, AssetHandle> mPathRegistry;
	};
}

#endif