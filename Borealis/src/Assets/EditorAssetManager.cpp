/******************************************************************************
/*!
\file       EditorAssetManager.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 23, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <yaml-cpp/yaml.h>
#include <Core/Core.hpp>
#include <Core/LoggerSystem.hpp>
#include <Assets/EditorAssetManager.hpp>
#include <Audio/Audio.hpp>
#include <Audio/AudioEngine.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Model.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>


namespace Borealis
{
	//TEMP
	//=====================================
	AssetMetaData DeserializeMetaFile(YAML::Node& node)
	{
		AssetMetaData metaData;

		metaData.name = node["Name"].as<std::string>();
		metaData.Handle = node["AssetHandle"].as<uint64_t>();
		metaData.Type = Asset::StringToAssetType(node["AssetType"].as<std::string>());
		std::string str = node["SourcePath"].as<std::string>();

		const std::string pattern = "..\\";
		size_t pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		//metaData.SourcePath = PathToAssetFolder.parent_path() / str;
		metaData.SourcePath = str;

		str = node["CachePath"].as<std::string>();

		pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		//metaData.CachePath = PathToAssetFolder.parent_path() / str;
		metaData.CachePath = str;
		//metaData.importDate = node["LastModifiedDate"].as<uint64_t>();

		return metaData;
	}

	void EditorAssetManager::LoadAssetRegistryRunTime(std::string path)
	{
		if (!std::filesystem::exists(path)) {
			BOREALIS_CORE_INFO("Registry file not found. Creating a new one");

			return;
		}

		std::ifstream registryFile(path);
		std::stringstream registryStream;
		registryStream << registryFile.rdbuf();
		registryFile.close();

		YAML::Node registryRoot = YAML::Load(registryStream.str());

		YAML::Node assetMetaInfo = registryRoot["Assets"];

		if (assetMetaInfo)
		{
			for (YAML::Node metaInfo : assetMetaInfo)
			{
				AssetMetaData metaData = DeserializeMetaFile(metaInfo);

				mAssetRegistry.insert({ metaData.Handle, metaData });
			}
		}
	}
	//=====================================

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle assetHandle)
	{
		if (!mAssetRegistry.contains(assetHandle))
		{
			BOREALIS_CORE_ERROR("NO Asset Handle : {}", assetHandle);
			return nullptr;
		}

		Ref<Asset> asset = nullptr;
		if (mLoadedAssets.contains(assetHandle))
		{
			asset = mLoadedAssets.at(assetHandle);
		}
		else
		{
			asset = LoadAsset(assetHandle);
			mLoadedAssets.insert({ assetHandle, asset });
		}
		BOREALIS_CORE_INFO("Get asset : {}", assetHandle);
		return asset;
	}

	void EditorAssetManager::SubmitAssetReloadRequest(AssetHandle assetHandle)
	{
		auto it = std::find_if(mAssetReloadRequests.begin(), mAssetReloadRequests.end(),
			[&assetHandle](const auto& request) {
				return request == assetHandle;
			});
		if(it == mAssetReloadRequests.end())
			mAssetReloadRequests.push_back(assetHandle);
	}

	Ref<Asset> EditorAssetManager::ReloadAsset(AssetHandle assetHandle)
	{
		//if not already loaded, no need to load it
		if (!mLoadedAssets.contains(assetHandle)) return nullptr;

		Ref<Asset> asset = LoadAsset(assetHandle);
		mLoadedAssets.at(assetHandle).swap(asset);// TODO: Temp until reload function is up

		return asset;
	}

	AssetMetaData const& EditorAssetManager::GetMetaData(AssetHandle assetHandle)
	{
		if (mAssetRegistry.contains(assetHandle))
		{
			return mAssetRegistry.at(assetHandle);
		}
		return{};
	}

	AssetRegistry& EditorAssetManager::GetAssetRegistry()
	{
		return mAssetRegistry;
	}

	//void EditorAssetManager::LoadRegistry(ProjectInfo projectInfo)
	//{
	//	//open registry database file
	//	//if !file create file
	//	if (!std::filesystem::exists(projectInfo.AssetsRegistryPath)) {
	//		BOREALIS_CORE_INFO("Registry file not found. Creating a new one");

	//		std::ofstream registry(projectInfo.AssetsRegistryPath);
	//		if (registry) {
	//			BOREALIS_CORE_INFO("Registry file created successfully.");
	//		}
	//		else {
	//			BOREALIS_CORE_ASSERT("Failed to create registry file.");
	//			return;
	//		}
	//		registry.close();
	//	}

	//	mAssetRegistryPath = projectInfo.AssetsRegistryPath;

	//	std::ifstream registryFile(projectInfo.AssetsRegistryPath);
	//	std::stringstream registryStream;
	//	registryStream << registryFile.rdbuf();
	//	registryFile.close();

	//	MetaFileSerializer::SetAssetFolderPath(projectInfo.AssetsRegistryPath);
	//	DeserializeRegistry(registryStream.str());


	//	//read files in assets folder and compare it with file
	//	// - check if the asset have a .meta file
	//	// - verify the version of the assets
	//	// - verify that asset have a cached if needed
	//	// - if every check is true, add to registry
	//	RegisterAllAssets(projectInfo.AssetsPath);

	//	SerializeRegistry();
	//}

	void EditorAssetManager::Clear()
	{
		mAssetRegistry.clear();
		mAssetRegistryPath.clear();
	}

	void EditorAssetManager::Update()
	{
		if (mAssetReloadRequests.empty()) return;

		for (AssetHandle assetHandle : mAssetReloadRequests)
		{
			ReloadAsset(assetHandle);
		}

		mAssetReloadRequests.clear();
	}

	Ref<Asset> GetModel(AssetConfig const& config, std::string const& path)
	{
		if (!GetConfig<MeshConfig>(config).skinMesh)
		{
			Model model;
			model.LoadModel(path);
			return MakeRef<Model>(model);
		}
		else // testing
		{
			SkinnedModel skinnedModel;
			skinnedModel.LoadModel(path);
			return MakeRef<SkinnedModel>(skinnedModel);
		}
	}

	Ref<Asset> EditorAssetManager::LoadAsset(AssetHandle assetHandle)
	{
		AssetMetaData metaData = mAssetRegistry.at(assetHandle);

		Ref<Asset> asset = nullptr;
		Animation anim;
		switch (metaData.Type)
		{
		case AssetType::Animation:
			anim.Load(metaData.SourcePath);
			asset = MakeRef<Animation>(anim);
			break;
		case AssetType::Audio:
			asset = MakeRef<Audio>(AudioEngine::LoadAudio(metaData.SourcePath.string()));
			break;
		case AssetType::Texture2D:
			asset = Texture2D::Create(metaData.CachePath.string());
			break;
		case AssetType::Material:
			asset = MakeRef<Material>(Material(metaData.SourcePath.string()));
			break;
		case AssetType::Mesh:
			asset = GetModel(metaData.Config, metaData.CachePath.string());
			break;
		case AssetType::BehaviourTree:
			asset = BTreeFactory::Instance().LoadBehaviourTree(metaData.SourcePath.string());
			break;
		default:
			BOREALIS_CORE_TRACE("Asset Type not handled for loadiing");
			return nullptr;
		}
		asset->mAssetHandle = assetHandle;
		return asset;
	}

	//void EditorAssetManager::SerializeRegistry()
	//{
	//	MetaFileSerializer::SerialzeRegistry(mAssetRegistryPath, mAssetRegistry);
	//}

	//void EditorAssetManager::DeserializeRegistry(std::string const& registryFileString)
	//{
	//	MetaFileSerializer::DeserializeRegistry(registryFileString, mAssetRegistry);
	//}

	//void EditorAssetManager::RegisterAsset(std::filesystem::path path)
	//{
	//	if (!VerifyMetaFile(path))
	//	{
	//		AssetMetaData meta = MetaFileSerializer::CreateAssetMetaFile(path);
	//		mAssetRegistry.insert({ meta.Handle, meta });
	//	}
	//}

	//void EditorAssetManager::RegisterAllAssets(std::filesystem::path path)
	//{
	//	if (!std::filesystem::exists(path))
	//	{
	//		BOREALIS_CORE_ASSERT("No such directory");
	//	}

	//	for (const auto& entry : std::filesystem::directory_iterator(path))
	//	{
	//		if (std::filesystem::is_directory(entry))
	//		{
	//			//check for existing meta file
	//			//if exist, check last modified date
	//			//if anything is wrong, create meta file
	//			if(!VerifyMetaFile(entry))
	//			{
	//				AssetMetaData meta = MetaFileSerializer::CreateAssetMetaFile(entry);
	//				mAssetRegistry.insert({ meta.Handle, meta });
	//			}

	//			RegisterAllAssets(entry.path());
	//		}
	//		else if (std::filesystem::is_regular_file(entry))
	//		{
	//			if (entry.path().extension().string() == ".meta")
	//			{
	//				continue;
	//			}

	//			RegisterAsset(entry.path());
	//		}
	//	}
	//}

	//bool EditorAssetManager::VerifyMetaFile(std::filesystem::path path)
	//{
	//	std::filesystem::path metaFilePath;
	//	if (std::filesystem::is_directory(path))
	//	{
	//		metaFilePath = path.string() + ".meta";
	//	}
	//	else
	//	{
	//		metaFilePath = path.replace_extension(".meta");
	//	}

	//	if (!std::filesystem::exists(metaFilePath))
	//	{
	//		return false;
	//	}
	//	else
	//	{
	//		AssetMetaData metaData = MetaFileSerializer::GetAssetMetaDataFile(metaFilePath);

	//		if (mAssetRegistry.contains(metaData.Handle))
	//		{
	//			if (mAssetRegistry.at(metaData.Handle).importDate == metaData.importDate)
	//			{
	//				return true;
	//			}
	//			else
	//			{
	//				BOREALIS_CORE_ASSERT("IMPORT DATE DIFF");
	//			}
	//		}
	//	}

	//	return false;
	//}
}

