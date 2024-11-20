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
#include <EditorAssets/AssetImporter.hpp>
#include <EditorAssets/MetaSerializer.hpp>

#include <Scripting/ScriptingSystem.hpp>
#include <thread>

namespace Borealis
{
	std::unique_ptr<filewatch::FileWatch<std::wstring>> fileWatcher = nullptr;
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
		mAssetRegistryPath = projectInfo.AssetsRegistryPath;

		std::ifstream registryFile(projectInfo.AssetsRegistryPath);
		std::stringstream registryStream;
		registryStream << registryFile.rdbuf();
		registryFile.close();

		MetaFileSerializer::SetAssetFolderPath(projectInfo.AssetsRegistryPath);
		AssetRegistry& assetRegistry = Project::GetEditorAssetsManager()->GetAssetRegistry();
		AssetRegistrySrcLoc& RegistrySrcLoc = Project::GetEditorAssetsManager()->GetAssetRegistrySrcLoc();
		//Deserialize metadata within registry into map
		DeserializeRegistry(registryStream.str(), assetRegistry, RegistrySrcLoc); //checked

		//Compare the metadata within registry with actual metadata for error checks
		std::set<AssetHandle> assetChecker;
		RegisterAllAssets(projectInfo.AssetsPath, assetRegistry, assetChecker);

		std::string originalPath = projectInfo.AssetsPath.string();
		originalPath.replace(originalPath.find("Assets"), std::string("Assets").length(), "Cache");
		std::filesystem::create_directories(originalPath); //create cache folder if not exist
		//ScriptingSystem::CompileCSharpQueue(originalPath + "/CSharp_Assembly.dll");
		//ScriptingSystem::LoadScriptAssemblies(originalPath + "/CSharp_Assembly.dll");
		ScriptingSystem::Reload({});

		std::set<AssetHandle> registryAssetHandles;
		for (auto& [handle, meta] : assetRegistry)
		{
			registryAssetHandles.insert(handle);
		}

		std::set<AssetHandle> result;
		auto list = std::set_difference(registryAssetHandles.begin(), registryAssetHandles.end(), assetChecker.begin(), assetChecker.end(),
						std::inserter(result, result.begin()));

		for (auto& handle : result)
		{
			assetRegistry.erase(handle);
		}

		SerializeRegistry();

		StartFileWatch();
	}
	
	AssetHandle AssetImporter::GetAssetHandle(std::filesystem::path const& path)
	{
		std::size_t hash = std::hash<std::string>{}(path.string());
		if (mPathRegistry.contains(hash)) return mPathRegistry.at(hash);

		return {};
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

		std::string command = compilerPath.string() + " " + sourcePath;

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
		//if failed to pass all error check, recompile if needed
		MetaErrorType errorType = VerifyMetaFile(path, assetRegistry);
		if (errorType != MetaErrorType::ALL_FINE)
		{
			std::filesystem::path dupPath = path.string() + ".meta";
			AssetMetaData meta;
			if (errorType == MetaErrorType::META_FILE_NOT_FOUND)
			{
				// If src does exist in Registry, use the same Asset Handle as before
				AssetRegistrySrcLoc& srcLocRegistry = Project::GetEditorAssetsManager()->GetAssetRegistrySrcLoc();
				if (srcLocRegistry.contains(path.string()))
				{
					meta = MetaFileSerializer::CreateAssetMetaFile(path, srcLocRegistry[path.string()]);
				}
				else
				{
					meta = MetaFileSerializer::CreateAssetMetaFile(path);
				}
			}
			else if (errorType == MetaErrorType::SOURCE_FILE_MODIFIED)
			{
				meta = MetaFileSerializer::GetAssetMetaDataFile(dupPath);
				AssetHandle handle = meta.Handle;
				meta = MetaFileSerializer::CreateAssetMetaFile(path, handle);
			}
			else
			{
				meta = MetaFileSerializer::GetAssetMetaDataFile(dupPath);
			}

			bool imported = false;
			std::filesystem::path metaPath = {};

			switch (meta.Type)
			{
			case AssetType::Mesh:
			case AssetType::Texture2D:
			case AssetType::Font:
				imported = ImportAsset(meta);
				metaPath = path;
				meta = MetaFileSerializer::GetAssetMetaDataFile(metaPath.string() + ".meta");
				break;
			default:
				break;
			}

			assetRegistry[meta.Handle] = meta;
			VerifyMetaFile(path, assetRegistry);
		}
		return GetAssetHandle(path);
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

	MetaErrorType AssetImporter::VerifyMetaFile(std::filesystem::path path, AssetRegistry& assetRegistry)
	{
		std::filesystem::path metaFilePath;

		std::size_t hash = std::hash<std::string>{}(path.string());

		if (!(path.extension() == ".meta"))
		{
			metaFilePath = path.string() + ".meta";
		}
		else
		{
			metaFilePath = path;
		}

		if (!std::filesystem::exists(metaFilePath))
		{
			return MetaErrorType::META_FILE_NOT_FOUND;
		}
		else
		{
			AssetMetaData metaData = MetaFileSerializer::GetAssetMetaDataFile(metaFilePath);

			if (assetRegistry.contains(metaData.Handle))
			{
				if (META_VERSION != metaData.Version)
				{
					//Version difference, update to latest version
					metaData = MetaFileSerializer::CreateAssetMetaFile(metaData.SourcePath, metaData.Handle);
				}

				//difference between source file and meta data
				if (MetaFileSerializer::HashFile(metaData.SourcePath) != metaData.SourceFileHash)
				{
					return MetaErrorType::SOURCE_FILE_MODIFIED;
				}
				//difference between current meta data and meta data within asset registry
				else if (assetRegistry.at(metaData.Handle).SourceFileHash != metaData.SourceFileHash)
				{
					return MetaErrorType::SOURCE_FILE_MODIFIED;
				}
				else
				{
					mPathRegistry.insert({ hash, metaData.Handle });
					return MetaErrorType::ALL_FINE;
				}
			}
		}

		return MetaErrorType::UNKNOWN;
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
					
					if (path.extension() == ".meta") return;
					
					assetHandleBuffer = GetAssetHandle(mAssetPath / path);
					assetMetaDataBuffer = Project::GetEditorAssetsManager()->GetMetaData(assetHandleBuffer);
					if (assetMetaDataBuffer.SourceFileHash == MetaFileSerializer::HashFile(mAssetPath / path)) return;
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

