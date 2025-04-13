/******************************************************************************
/*!
\file       EditorAssetManager.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 23, 2024
\brief      class for Editor Asset Manager

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef EditorAssetManager_HPP
#define EditorAssetManager_HPP

#include <unordered_map>
#include <filesystem>

#include <Core/ProjectInfo.hpp>
#include <Assets/IAssetManager.hpp>
#include <Assets/AssetMetaData.hpp>

namespace Borealis
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetaData>;
	using AssetRegistrySrcLoc = std::unordered_map<std::string, AssetHandle>;
	using AssetLoaderFunc = std::function<Ref<Asset>(std::filesystem::path const&, AssetMetaData const&)>;
	using AssetReloadFunc = std::function<void(AssetMetaData const&, Ref<Asset> const&)>;

	class EditorAssetManager : public IAssetManager
	{
	public:

		//TEMP
		//===================================
		void LoadAssetRegistryRunTime(std::string path, bool encrypt = false);
		//===================================

		void RegisterAsset(AssetType type, AssetLoaderFunc loadFunc, AssetReloadFunc reloadFunc);

		/*!***********************************************************************
			\brief
				Get asset by handle
		*************************************************************************/
		Ref<Asset> GetAsset(AssetHandle assetHandle) override;

		void SubmitAssetReloadRequest(AssetHandle assetHandle);

		Ref<Asset> ReloadAsset(AssetHandle assetHandle);

		/*!***********************************************************************
			\brief
				Get meta data by handel
		*************************************************************************/
		AssetMetaData const& GetMetaData(AssetHandle assetHandle) const;

		AssetMetaData & GetMetaData(AssetHandle assetHandle);

		/*!***********************************************************************
			\brief
				Get asset registry
		*************************************************************************/
		AssetRegistry& GetAssetRegistry();

		AssetRegistrySrcLoc& GetAssetRegistrySrcLoc();

		/*!***********************************************************************
			\brief
				Clear the asset manager
		*************************************************************************/
		void Clear();

		void Init(ProjectInfo const& info);

		void Update();

	private:

		/*!***********************************************************************
			\brief
				Load asset by handle
		*************************************************************************/
		Ref<Asset> LoadAsset(AssetHandle assetHandle);

	private:
		std::filesystem::path mAssetRegistryPath;
		std::filesystem::path mCachePath;
		AssetRegistry mAssetRegistry;
		AssetRegistrySrcLoc mAssetRegistrySrcLoc;
		std::unordered_map<AssetHandle, Ref<Asset>> mLoadedAssets;

		std::list<AssetHandle> mAssetReloadRequests;

		std::unordered_map<AssetType, AssetLoaderFunc> mAssetLoaders;
		std::unordered_map<AssetType, AssetReloadFunc> mAssetReloaders;
	};
}

#endif
