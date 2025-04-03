/******************************************************************************
/*!
\file       AssetManager.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetManager_HPP
#define AssetManager_HPP

#include <Core/Core.hpp>
#include <Core/Project.hpp>
#include <Core/ApplicationManager.hpp>
#include <Scripting/ScriptingSystem.hpp>

namespace Borealis
{
	struct AssetInfo
	{
		AssetType type;
		std::string name;
		std::vector<std::string> extensions;
		AssetLoaderFunc loadFunc;
		AssetReloadFunc reloadFunc = nullptr;
	};

	class AssetManager
	{
	public:
		static void RegisterAllAssetType();

		static void RegisterAsset(AssetInfo const& assetInfo);
		

		/*!***********************************************************************
			\brief
				Get Asset by handle
		*************************************************************************/
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Project::GetEditorAssetsManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		/*!***********************************************************************
			\brief
				Get meta data by handle
		*************************************************************************/
		static AssetMetaData & GetMetaData(AssetHandle handle)
		{
			return Project::GetEditorAssetsManager()->GetMetaData(handle);
		}

		static AssetType GetAssetTypeFromExtension(std::filesystem::path path);

		static std::string AssetTypeToString(AssetType type);

		static AssetType StringToAssetType(std::string const& type);

		static void VerifyTexture(AssetHandle handle);

		//TEMP
		//===============================================================
		static void InsertMetaData(AssetMetaData data)
		{
			Project::GetEditorAssetsManager()->GetAssetRegistry().insert({ data.Handle, data });
		}

		static void SetRunTime()
		{
			AssetManager::RegisterAllAssetType();
			Project::GetEditorAssetsManager()->Init(Project::GetProjectInfo());
			Project::GetEditorAssetsManager()->LoadAssetRegistryRunTime("AssetRegistry.brdb");
			ScriptingSystem::LoadScriptAssembliesNonThreaded("Cache/CSharp_Assembly.dll");
		}

	private:
		inline static std::unordered_map<std::string, AssetType> extensionToAssetType;
		inline static std::unordered_map<AssetType, std::string> assetTypeToString;
		inline static std::unordered_map<std::string, AssetType> stringToAssetType;
	};
}

#endif
