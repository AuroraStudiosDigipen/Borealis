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
#include <Core/LoggerSystem.hpp>

#include <Core/Project.hpp>

#include <Assets/Asset.hpp>
#include <EditorAssets/AssetImporter.hpp>
#include <EditorAssets/MetaSerializer.hpp>

#include <thread>
namespace Borealis
{
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

		mAssetRegistryPath = projectInfo.AssetsRegistryPath;
		mAssetRegistryPathStatic = projectInfo.AssetsRegistryPath;

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

		SerializeRegistry();
	}

	AssetHandle AssetImporter::GetAssetHandle(std::filesystem::path const& path)
	{
		std::size_t hash = std::hash<std::string>{}(path.string());
		if (mPathRegistry.contains(hash)) return mPathRegistry.at(hash);

		//TEMP==============================================================================
		BOREALIS_CORE_TRACE("Inserting file into registry (AssetImporter::GetAssetHandle)");
		AssetRegistry& assetRegistry = Project::GetEditorAssetsManager()->GetAssetRegistry();
		AssetMetaData meta = MetaFileSerializer::CreateAssetMetaFile(path);

		bool imported = false;
		std::filesystem::path metaPath = {};

		switch (meta.Type)
		{
		case AssetType::Mesh:
		case AssetType::Texture2D:
		case AssetType::Font:
			imported = ImportAssetTEMP(meta);
			metaPath = path;
			meta = MetaFileSerializer::GetAssetMetaDataFile(metaPath.replace_extension(".meta"));
			break;
		default:
			break;
		}
		assetRegistry.insert({ meta.Handle, meta });
		mPathRegistry.insert({ hash, meta.Handle });
		MetaFileSerializer::SerialzeRegistry(mAssetRegistryPathStatic, assetRegistry);
		return meta.Handle;
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
		std::string assetType = Asset::AssetTypeToString(metaData.Type);
		std::string assetHandle = std::to_string(metaData.Handle);

		std::string command = compilerPath.string() + " " + sourcePath;// +" " + assetType + " " + assetHandle;

		int result = system(command.c_str());

		return false;
	}

	bool AssetImporter::ImportAssetTEMP(AssetMetaData metaData)
	{
		//check if assets needs to be imported

		//if yes pass info to compiler

		//system()

		std::filesystem::path compilerPath = std::filesystem::canonical("BorealisAssetCompiler.exe");
		std::string sourcePath = metaData.SourcePath.string();
		std::string assetType = Asset::AssetTypeToString(metaData.Type);
		std::string assetHandle = std::to_string(metaData.Handle);

		std::string command = compilerPath.string() + " " + sourcePath;// +" " + assetType + " " + assetHandle;

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
				meta = MetaFileSerializer::CreateAssetMetaFile(dupPath);
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
				meta = MetaFileSerializer::GetAssetMetaDataFile(metaPath.replace_extension(".meta"));
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

		if (std::filesystem::is_directory(path))
		{
			metaFilePath = path.string() + ".meta";
		}
		else
		{
			metaFilePath = path.replace_extension(".meta");
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
}

