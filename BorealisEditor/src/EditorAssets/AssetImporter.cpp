/******************************************************************************
/*!
\file       AssetImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <FileWatch.hpp>
#include <Core/LoggerSystem.hpp>

#include <Core/Project.hpp>

#include <Assets/Asset.hpp>
#include <Assets/AssetManager.hpp>
#include <EditorAssets/AssetImporter.hpp>
#include <EditorAssets/MetaSerializer.hpp>

#include <assimp/zlib.h>
#include <Scripting/ScriptingSystem.hpp>
#include <thread>

namespace Borealis
{
	std::unique_ptr<filewatch::FileWatch<std::wstring>> fileWatcher = nullptr;
	void AssetImporter::Update()
	{
		if (mQueue.empty()) return;

		for (AssetMetaData const& metaData : mQueue)
		{
			ImportAsset(metaData);
			AssetMetaData meta = MetaFileSerializer::GetAssetMetaDataFile(metaData.SourcePath.string() + ".meta");
			AssetManager::InsertMetaData(meta);
			AssetImporter::SerializeRegistry();

			Project::GetEditorAssetsManager()->SubmitAssetReloadRequest(metaData.Handle);
		}

		mQueue.clear();
	}
	void AssetImporter::LoadRegistry(Borealis::ProjectInfo projectInfo)
	{
		//open registry database file
		//if !file create file
		if (!std::filesystem::exists(projectInfo.AssetsRegistryPath)) {
			BOREALIS_CORE_INFO("Registry file not found. Creating a new one");

			std::ofstream registry(projectInfo.AssetsRegistryPath);
			if (registry) {
				BOREALIS_CORE_INFO("Registry file created successfully.");
			}
			else {
				BOREALIS_CORE_ASSERT(false, "Failed to create registry file.");
				return;
			}
			registry.close();
		}

		mAssetPath = projectInfo.AssetsPath;
		mCachePath = mAssetPath;
		mCachePath.replace_filename("Cache");
		mAssetRegistryPath = projectInfo.AssetsRegistryPath;

		Project::GetEditorAssetsManager()->Init(projectInfo);

		AssetManager::RegisterAllAssetType();

		std::ifstream registryFile(projectInfo.AssetsRegistryPath);
		std::stringstream registryStream;
		registryStream << registryFile.rdbuf();
		registryFile.close();

		MetaFileSerializer::SetAssetFolderPath(projectInfo.AssetsRegistryPath);
		AssetRegistry& assetRegistry = Project::GetEditorAssetsManager()->GetAssetRegistry();
		AssetRegistrySrcLoc& RegistrySrcLoc = Project::GetEditorAssetsManager()->GetAssetRegistrySrcLoc();
		//Deserialize metadata within registry into map

		std::string originalPath = projectInfo.AssetsPath.string();
		originalPath.replace(originalPath.find("Assets"), std::string("Assets").length(), "Cache");
		std::filesystem::create_directories(originalPath); //create cache folder if not exist

		DeserializeRegistry(registryStream.str(), assetRegistry, RegistrySrcLoc); //checked

		StartFileWatch();

		//Compare the metadata within registry with actual metadata for error checks
		std::set<AssetHandle> assetChecker;
		RegisterAllAssets(projectInfo.AssetsPath, assetRegistry, assetChecker);

		ScriptingSystem::Reload({});

		//Removed for now, implement again in future
		//std::set<AssetHandle> registryAssetHandles;
		//for (auto& [handle, meta] : assetRegistry)
		//{
		//	registryAssetHandles.insert(handle);
		//}

		//std::set<AssetHandle> result;
		//auto list = std::set_difference(registryAssetHandles.begin(), registryAssetHandles.end(), assetChecker.begin(), assetChecker.end(),
		//				std::inserter(result, result.begin()));

		//for (auto& handle : result)
		//{
		//	assetRegistry.erase(handle);
		//}

		SerializeRegistry();
	}
	
	AssetHandle AssetImporter::GetAssetHandle(std::filesystem::path const& path)
	{
		std::size_t hash = std::hash<std::string>{}(path.string());
		if (mPathRegistry.contains(hash)) return mPathRegistry.at(hash);

		return {};
	}

	void AssetImporter::AddToRecompileQueue(AssetMetaData metaData)
	{
		mQueue.push_back(metaData);
	}

	void AssetImporter::InsertAssetHandle(std::filesystem::path const& path, AssetHandle handle)
	{
		std::size_t hash = std::hash<std::string>{}(path.string());
		mPathRegistry.insert({ hash,handle });
	}

	bool AssetImporter::ImportAsset(AssetMetaData metaData)
	{
		//check if assets needs to be imported

		//if yes pass info to compiler

		//system()

		std::filesystem::path compilerPath = std::filesystem::canonical("BorealisAssetCompiler.exe");
		std::string sourcePath = metaData.SourcePath.string();

		// Enclose the paths in quotes to handle spaces
		std::string command = compilerPath.string() + " \"" + sourcePath + "\"";

		BOREALIS_CORE_INFO("{}", command);

		// Execute the command
		int result = system(command.c_str());

		return false;
	}

	void AssetImporter::SerializeRegistry()
	{
		MetaFileSerializer::SerialzeRegistry(mAssetRegistryPath, Project::GetEditorAssetsManager()->GetAssetRegistry());
	}

	void AssetImporter::DeserializeRegistry(std::string const& registryFileString, AssetRegistry& assetRegistry, AssetRegistrySrcLoc& RegistrySrcLoc)
	{
		MetaFileSerializer::DeserializeRegistry(registryFileString, assetRegistry, RegistrySrcLoc);
	}

	AssetHandle AssetImporter::RegisterAsset(std::filesystem::path path, AssetRegistry& assetRegistry)
	{
		AssetMetaData metaData;
		bool metaExist = CheckForMetaFile(path, assetRegistry, metaData);
		if(metaExist)
		{
			MetaErrorType errorType = VerifyMetaFile(metaData);
			if (errorType != MetaErrorType::ALL_FINE)
			{
				HandleError(errorType, metaData);
			}
		}
		else
		{
			if(!std::filesystem::is_directory(metaData.SourcePath))
				CreateCache(metaData);
			assetRegistry.at(metaData.Handle) = MetaFileSerializer::GetAssetMetaDataFile(metaData.SourcePath.string() + ".meta");
		}

		std::size_t hash = std::hash<std::string>{}(path.string());
		mPathRegistry.insert({ hash, metaData.Handle });

		return metaData.Handle;
	}

	void AssetImporter::RegisterAllAssets(std::filesystem::path path, AssetRegistry& assetRegistry, std::set<AssetHandle>& assetChecker)
	{
		if (!std::filesystem::exists(path))
		{
			BOREALIS_CORE_ASSERT(false, "No such directory");
		}

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_directory(entry))
			{
				//check for existing meta file
				//if exist, check last modified date
				//if anything is wrong, create meta file
				assetChecker.insert(RegisterAsset(entry, assetRegistry));

				RegisterAllAssets(entry.path(), assetRegistry, assetChecker);
			}
			else if (std::filesystem::is_regular_file(entry))
			{
				if (entry.path().extension().string() == ".meta")
				{
					continue;
				}

				assetChecker.insert(RegisterAsset(entry.path(), assetRegistry));
			}
		}
	}

	bool AssetImporter::CheckForMetaFile(std::filesystem::path const& filepath, AssetRegistry& assetRegistry, AssetMetaData& metaData)
	{
		std::filesystem::path metaFilePath;

		if (!(filepath.extension() == ".meta"))
		{
			metaFilePath = filepath.string() + ".meta";
		}
		else // prob nvr hit
		{
			metaFilePath = filepath;
		}

		if (!std::filesystem::exists(metaFilePath))
		{
			//create new meta file
			//register into asset registry
			metaData = MetaFileSerializer::CreateAssetMetaFile(filepath);
			assetRegistry.insert({ metaData.Handle, metaData });
			return false;
		}
		//get from asset registry
		metaData = MetaFileSerializer::GetAssetMetaDataFile(metaFilePath);
		return true;
	}

	MetaErrorType AssetImporter::VerifyMetaFile(AssetMetaData& metaData)
	{
		std::filesystem::path metaFilePath;

		if (META_VERSION != metaData.Version)
		{
			//Version difference, update to latest version
			metaData = MetaFileSerializer::CreateAssetMetaFile(metaData.SourcePath, metaData.Handle);
		}

		//difference between source file and meta data
		uint32_t newHash = MetaFileSerializer::HashFile(metaData.SourcePath);
		if (newHash != metaData.SourceFileHash)
		{
			return MetaErrorType::SOURCE_FILE_MODIFIED;
		}
		// once reformatted, copy all assets into cache folder
		// add the copy function into meta file creation
		// 
		std::filesystem::path cacheFilePath = mCachePath;
		cacheFilePath.append(std::to_string(metaData.Handle));
		
		if (!std::filesystem::exists(cacheFilePath) && !std::filesystem::is_directory(metaData.SourcePath))
		{
			return MetaErrorType::CACHE_FILE_NOT_FOUND;
		}
		else
		{
			//mPathRegistry.insert({ hash, metaData.Handle });
			return MetaErrorType::ALL_FINE;
		}

		return MetaErrorType::UNKNOWN;
	}

	void AssetImporter::HandleError(MetaErrorType errorType, AssetMetaData & metaData)
	{
		if (errorType == MetaErrorType::SOURCE_FILE_MODIFIED || errorType == MetaErrorType::CACHE_FILE_NOT_FOUND)
		{
			CreateCache(metaData);
		}

		metaData.SourceFileHash = MetaFileSerializer::HashFile(metaData.SourcePath);
		MetaFileSerializer::SaveMetaFile(metaData);
		//Update meta file
	}

	void AssetImporter::CopyToCacheFolder(AssetMetaData& metaData)
	{
		//Copy file and move to cache folder
		metaData.SourcePath;
		//rename to metaData.handle;
		//create at cache path, move to parent folder until the parent of Asset folder and copy into cache folder
		std::filesystem::path cachePath = mCachePath;
		cachePath /= std::to_string(metaData.Handle);

		std::filesystem::copy(metaData.SourcePath, cachePath, std::filesystem::copy_options::overwrite_existing);
	}

	void AssetImporter::CreateCache(AssetMetaData& metaData)
	{
		//get source path
		//create cache with assethandle being the name
		BOREALIS_CORE_INFO("Create cache");

		switch (metaData.Type)
		{
		case AssetType::Mesh:
		case AssetType::Texture2D:
		case AssetType::Font:
			ImportAsset(metaData);
			//get updated meta file
			break;
		default:
			CopyToCacheFolder(metaData);
			break;
		}
	}

	void AssetImporter::StartFileWatch()
	{
		filewatch::FileWatch<std::wstring> watchBuffer(
			mAssetPath.generic_wstring(),
			[this](const std::filesystem::path& path, const filewatch::Event change_type) {
				AssetRegistry* assetRegistry = nullptr;
				AssetHandle assetHandleBuffer = -1;
				AssetMetaData assetMetaDataBuffer;
				BOREALIS_CORE_INFO("File change detected : {}", path.string());
				switch (change_type)
				{
				case filewatch::Event::added:
					if (path.extension() == ".meta") return;
					assetRegistry = &Project::GetEditorAssetsManager()->GetAssetRegistry();
					RegisterAsset(mAssetPath / path, *assetRegistry);
					break;
				case filewatch::Event::removed:
					//if .meta is deleted, find related meta data in registry and re-serialize it
					//if src file is deleted
					//replace extention with .meta
					// remove from registry
					break;
				case filewatch::Event::modified:
					//compare modified file with meta file
					//check if need to re compile
					
					assetHandleBuffer = GetAssetHandle(mAssetPath / path);
					assetMetaDataBuffer = Project::GetEditorAssetsManager()->GetMetaData(assetHandleBuffer);
					//if (assetMetaDataBuffer.SourceFileHash == MetaFileSerializer::HashFile(mAssetPath / path)) return;
					ImportAsset(assetMetaDataBuffer);

					//dont call reload in filewatch as multithreading, submit a reload request and handle it that way
					Project::GetEditorAssetsManager()->SubmitAssetReloadRequest(assetHandleBuffer);

					break;
				case filewatch::Event::renamed_old:
					//handle rename
					break;
				case filewatch::Event::renamed_new:
					//handle rename
					break;
				};
				SerializeRegistry();
			}
		);

		fileWatcher = std::make_unique<filewatch::FileWatch<std::wstring>>(watchBuffer);
	}
}

