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
	enum class MetaErrorType
	{
		ALL_FINE,
		META_FILE_NOT_FOUND,
		SOURCE_FILE_MODIFIED,
		CACHE_FILE_NOT_FOUND,
		UNKNOWN
	};

	class AssetImporter
	{
	public:
		~AssetImporter() { StopFileWatch(); }


		void Update();
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

		static void AddToRecompileQueue(AssetMetaData metaData);

		//TEMP
		static void InsertAssetHandle(std::filesystem::path const& path, AssetHandle handle);
	private:

		static void StopFileWatch();

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
		void DeserializeRegistry(std::string const& registryFileString, AssetRegistry& assetRegistry, AssetRegistrySrcLoc& RegistrySrcLoc);

		/*!***********************************************************************
			\brief
				Register asset
			\return
				Asset handle of the registered asset
		*************************************************************************/
		AssetHandle RegisterAsset(std::filesystem::path path, AssetRegistry& assetRegistry);

		/*!***********************************************************************
			\brief
				register all asset in a path
		*************************************************************************/
		void RegisterAllAssets(std::filesystem::path path, AssetRegistry& assetRegistry, std::set<AssetHandle>& assetChecker);

		/*!***********************************************************************
			\brief
				check for meta file
		*************************************************************************/

		bool CheckForMetaFile(std::filesystem::path const& filepath, AssetRegistry& assetRegistry, AssetMetaData& metaData);

		/*!***********************************************************************
			\brief
				verify meta file
		*************************************************************************/
		MetaErrorType VerifyMetaFile(AssetMetaData& metaData);


		/*!***********************************************************************
			\brief
				Handle various meta file errors/mismatch
		*************************************************************************/

		void HandleError(MetaErrorType errorType, AssetMetaData& metaData);

		void CopyToCacheFolder(AssetMetaData& metaData);

		/*!***********************************************************************
			\brief
				Create Cache file
		*************************************************************************/

		void CreateCache(AssetMetaData& metaData);

		void StartFileWatch();


	private:
		std::filesystem::path mAssetPath;
		std::filesystem::path mCachePath;
		std::filesystem::path mAssetRegistryPath;
		inline static std::unordered_map<std::size_t, AssetHandle> mPathRegistry;
		inline static std::list<AssetMetaData> mQueue;
	};
}

#endif
