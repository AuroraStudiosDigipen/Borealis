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
#include <Assets/AssetManager.hpp>
#include <Assets/EditorAssetManager.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Model.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <mutex>
#include <openssl/evp.h>
namespace Borealis
{

	static std::mutex reloadMutex;
	//TEMP
	//=====================================
	namespace
	{

		static bool decryptFileToStream(const std::string& inputPath,
			unsigned char* key, unsigned char* iv,
			std::stringstream& decryptedStream) {
			std::ifstream inFile(inputPath, std::ios::binary);
			if (!inFile) return false;

			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
			if (!ctx) return false;

			EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

			const size_t bufferSize = 4096;
			std::vector<unsigned char> inBuf(bufferSize);
			std::vector<unsigned char> outBuf(bufferSize);

			int outLen = 0;
			while (inFile.good()) {
				inFile.read(reinterpret_cast<char*>(inBuf.data()), bufferSize);
				std::streamsize readBytes = inFile.gcount();

				if (!EVP_DecryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(readBytes))) {
					EVP_CIPHER_CTX_free(ctx);
					return false;
				}

				// Write decrypted data to stringstream
				decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);
			}

			if (!EVP_DecryptFinal_ex(ctx, outBuf.data(), &outLen)) {
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}

			// Write the final decrypted data to stringstream
			decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);

			EVP_CIPHER_CTX_free(ctx);
			return true;
		}

		TextureType GetTextureType(std::string const& typeStr)
		{
			if (typeStr == "Default") return TextureType::_DEFAULT;
			if (typeStr == "Normal Map") return TextureType::_NORMAL_MAP;
			return TextureType::_DEFAULT;
		}

		TextureShape GetTextureShape(std::string const& typeStr)
		{
			if (typeStr == "2D") return TextureShape::_2D;
			if (typeStr == "Cube") return TextureShape::_CUBE;
			return TextureShape::_2D;
		}

		TextureConfig DeserializeTextureConfig(YAML::Node& node)
		{
			TextureConfig config;
			if (node["TextureType"])
				config.type = GetTextureType(node["TextureType"].as<std::string>());
			if (node["TextureShape"])
				config.shape = GetTextureShape(node["TextureShape"].as<std::string>());
			if (node["sRGB"])
				config.sRGB = node["sRGB"].as<bool>();
			if (node["MipMaps"])
				config.generateMipMaps = node["MipMaps"].as<bool>();

			return config;
		}

		MeshConfig DeserializeMeshConfig(YAML::Node& node)
		{
			MeshConfig config;
			if (node["IsSkinnedMesh"])
				config.skinMesh = node["IsSkinnedMesh"].as<bool>();
			else
				config.skinMesh = false;

			return config;
		}

		AssetConfig DeserializeMetaConfigFile(YAML::Node& node, AssetType type)
		{
			AssetConfig config{};

			switch (type)
			{
			case Borealis::AssetType::None:
				break;
			case Borealis::AssetType::Mesh:
				config = DeserializeMeshConfig(node);
				break;
			case Borealis::AssetType::Shader:
				break;
			case Borealis::AssetType::Texture2D:
				config = DeserializeTextureConfig(node);
				break;
			case Borealis::AssetType::Folder:
				break;
			case Borealis::AssetType::Font:
				break;
			case Borealis::AssetType::Scene:
				break;
			case Borealis::AssetType::Material:
				break;
			case Borealis::AssetType::Prefab:
				break;
			default:
				break;
			}

			return config;
		}
	}
	
	AssetMetaData DeserializeMetaFile(YAML::Node& node)
	{
		AssetMetaData metaData;

		metaData.name = node["Name"].as<std::string>();
		metaData.Handle = node["AssetHandle"].as<uint64_t>();
		metaData.Type = AssetManager::StringToAssetType(node["AssetType"].as<std::string>());
		metaData.Config = DeserializeMetaConfigFile(node, metaData.Type);
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

	//=================================ABOVE IS TEMP==================

	void EditorAssetManager::LoadAssetRegistryRunTime(std::string path, bool encrypt)
	{
		if (!std::filesystem::exists(path)) {
			BOREALIS_CORE_INFO("Registry file not found. Creating a new one");

			return;
		}

		std::stringstream registryStream;
		if (encrypt)
		{
			decryptFileToStream(path, Project::assembleKey().data(), Project::assembleIV().data(), registryStream);
		}
		else
		{
			std::ifstream registryFile(path);
			registryStream << registryFile.rdbuf();
			registryFile.close();
		}

		YAML::Node registryRoot = YAML::Load(registryStream.str());

		YAML::Node assetMetaInfo = registryRoot["Assets"];

		if (assetMetaInfo)
		{
			for (YAML::Node metaInfo : assetMetaInfo)
			{
				AssetMetaData metaData = DeserializeMetaFile(metaInfo);

				mAssetRegistry.insert({ metaData.Handle, metaData });
				mAssetRegistrySrcLoc.insert({metaData.SourcePath.string(), metaData.Handle});
			}
		}
	}

	//=====================================

	void EditorAssetManager::RegisterAsset(AssetType type, AssetLoaderFunc loadFunc, AssetReloadFunc reloadFunc)
	{
		if (loadFunc != nullptr)
		{
			mAssetLoaders.insert({ type, loadFunc });
		}

		if (reloadFunc != nullptr)
		{
			mAssetReloaders.insert({ type, reloadFunc });
		}
	}

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
		return asset;
	}

	void EditorAssetManager::SubmitAssetReloadRequest(AssetHandle assetHandle)
	{
		auto it = std::find_if(mAssetReloadRequests.begin(), mAssetReloadRequests.end(),
			[&assetHandle](const auto& request) {
				return request == assetHandle;
			});
		if(it == mAssetReloadRequests.end())
		{
			std::lock_guard<std::mutex> lock(reloadMutex);
			mAssetReloadRequests.push_back(assetHandle);
		}
	}

	Ref<Asset> EditorAssetManager::ReloadAsset(AssetHandle assetHandle)
	{
		AssetMetaData const& assetMetaData = GetMetaData(assetHandle);

		if (mAssetReloaders.contains(assetMetaData.Type))
		{
			if (assetMetaData.Type == AssetType::Script)
			{
				ScriptingSystem::Reload(assetMetaData);
			}
			else if (mLoadedAssets.contains(assetHandle))
			{
				mAssetReloaders[assetMetaData.Type](assetMetaData, mLoadedAssets[assetHandle]);
				return mLoadedAssets[assetHandle];
			}
		}
		//if not already loaded, no need to load it
		return Ref<Asset>();
	}

	AssetMetaData const& EditorAssetManager::GetMetaData(AssetHandle assetHandle) const
	{
		if (mAssetRegistry.contains(assetHandle))
		{
			return mAssetRegistry.at(assetHandle);
		}
		return{};
	}

	AssetMetaData& EditorAssetManager::GetMetaData(AssetHandle assetHandle)
	{
		if (mAssetRegistry.contains(assetHandle))
		{
			return mAssetRegistry.at(assetHandle);
		}

		static AssetMetaData empty{};
		return empty;
	}

	AssetRegistry& EditorAssetManager::GetAssetRegistry()
	{
		return mAssetRegistry;
	}

	AssetRegistrySrcLoc& EditorAssetManager::GetAssetRegistrySrcLoc()
	{
		return mAssetRegistrySrcLoc;
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
		mAssetRegistrySrcLoc.clear();
	}

	void EditorAssetManager::Init(ProjectInfo const& info)
	{
		mCachePath = info.CachePath;
	}

	void EditorAssetManager::Update()
	{
		std::lock_guard<std::mutex> lock(reloadMutex);

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

		if (mAssetLoaders.contains(metaData.Type))
		{
			asset = mAssetLoaders[metaData.Type](mCachePath, metaData);
		}

		if (asset == nullptr)
		{
			//asset = GetDefaultAsset();
		}

		asset->mAssetHandle = assetHandle;
		return asset;
	}
}

