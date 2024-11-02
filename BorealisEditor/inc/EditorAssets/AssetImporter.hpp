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
		/*!***********************************************************************
			\brief
				Load registry
		*************************************************************************/
		void LoadRegistry(Borealis::ProjectInfo projectInfo);

		/*!***********************************************************************
			\brief
				Get asset handle from path
		*************************************************************************/
		static AssetHandle GetAssetHandle(std::filesystem::path const& path);

		//TEMP
		static void InsertAssetHandle(std::filesystem::path const& path, AssetHandle handle);

	private:
		/*!***********************************************************************
			\brief
				Import Asset through compiler
		*************************************************************************/
		bool ImportAsset(AssetMetaData metaData);

		/*!***********************************************************************
			\brief
				Serialize entire registry
		*************************************************************************/
		void SerializeRegistry();

		/*!***********************************************************************
			\brief
				Deserialize registry
		*************************************************************************/
		void DeserializeRegistry(std::string const& registryFileString, AssetRegistry& assetRegistry);

		/*!***********************************************************************
			\brief
				Register asset
		*************************************************************************/
		void RegisterAsset(std::filesystem::path path, AssetRegistry& assetRegistry);

		/*!***********************************************************************
			\brief
				register all asset in a path
		*************************************************************************/
		void RegisterAllAssets(std::filesystem::path path, AssetRegistry& assetRegistry);

		/*!***********************************************************************
			\brief
				verify meta file
		*************************************************************************/
		bool VerifyMetaFile(std::filesystem::path path, AssetRegistry& assetRegistry);

		void StartFileWatch();


	private:
		std::filesystem::path mAssetPath;
		std::filesystem::path mAssetRegistryPath;
		inline static std::unordered_map<std::size_t, AssetHandle> mPathRegistry;
	};
}

#endif
