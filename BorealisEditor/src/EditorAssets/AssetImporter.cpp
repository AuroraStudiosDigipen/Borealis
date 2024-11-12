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
		DeserializeRegistry(registryStream.str(), assetRegistry);


		//read files in assets folder and compare it with file
		// - check if the asset have a .meta file
		// - verify the version of the assets
		// - verify that asset have a cached if needed
		// - if every check is true, add to registry
		RegisterAllAssets(projectInfo.AssetsPath, assetRegistry);

		std::string originalPath = projectInfo.AssetsPath.string();
		originalPath.replace(originalPath.find("Assets"), std::string("Assets").length(), "Cache");
		ScriptingSystem::CompileCSharpQueue(originalPath + "/CSharp_Assembly.dll");
		ScriptingSystem::LoadScriptAssemblies(originalPath + "/CSharp_Assembly.dll");

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

	void AssetImporter::DeserializeRegistry(std::string const& registryFileString, AssetRegistry& assetRegistry)
	{
		MetaFileSerializer::DeserializeRegistry(registryFileString, assetRegistry);
	}

	void AssetImporter::RegisterAsset(std::filesystem::path path, AssetRegistry& assetRegistry)
	{
		if (!VerifyMetaFile(path, assetRegistry))
		{
			auto dupPath = path;
			AssetMetaData meta;
			if (!std::filesystem::exists(dupPath.replace_extension(".meta").string()))
			{
				meta = MetaFileSerializer::CreateAssetMetaFile(path);
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
			case AssetType::Script:
				ScriptingSystem::PushCSharpQueue(path.string());
				break;
			default:
				break;
			}

			assetRegistry.insert({ meta.Handle, meta });
			VerifyMetaFile(path, assetRegistry);
		}
	}

	void AssetImporter::RegisterAllAssets(std::filesystem::path path, AssetRegistry& assetRegistry)
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
				RegisterAsset(entry, assetRegistry);

				RegisterAllAssets(entry.path(), assetRegistry);
			}
			else if (std::filesystem::is_regular_file(entry))
			{
				if (entry.path().extension().string() == ".meta")
				{
					continue;
				}

				RegisterAsset(entry.path(), assetRegistry);
			}
		}
	}

	bool AssetImporter::VerifyMetaFile(std::filesystem::path path, AssetRegistry& assetRegistry)
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
			return false;
		}
		else
		{
			AssetMetaData metaData = MetaFileSerializer::GetAssetMetaDataFile(metaFilePath);

			if (assetRegistry.contains(metaData.Handle))
			{
				if (assetRegistry.at(metaData.Handle).importDate == metaData.importDate)
				{
					mPathRegistry.insert({ hash, metaData.Handle });
					return true;
				}
				else
				{
					BOREALIS_CORE_ASSERT(false, "IMPORT DATE DIFF");
				}
			}
		}

		return false;
	}
	void AssetImporter::StartFileWatch()
	{
		filewatch::FileWatch<std::wstring> watchBuffer(
			mAssetPath.generic_wstring(),
			[this](const std::filesystem::path& path, const filewatch::Event change_type) {
				AssetRegistry* assetRegistry = nullptr;
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

