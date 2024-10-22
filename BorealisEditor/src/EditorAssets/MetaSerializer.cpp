/******************************************************************************
/*!
\file       MetaSerializer.cpp
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
#include <Core/LoggerSystem.hpp>
#include <EditorAssets/MetaSerializer.hpp>

#include <yaml-cpp/yaml.h>


namespace Borealis
{
	void MetaFileSerializer::SetAssetFolderPath(std::filesystem::path const& path)
	{
		PathToAssetFolder = path;
	}

	void SerializeMetaFile(YAML::Emitter& out, AssetMetaData const& assetMetaData, std::filesystem::path PathToAssetFolder)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << assetMetaData.name;
		out << YAML::Key << "AssetHandle" << YAML::Value << assetMetaData.Handle;
		out << YAML::Key << "AssetType" << YAML::Value << Asset::AssetTypeToString(assetMetaData.Type);
		//out << YAML::Key << "SourcePath" << YAML::Value << assetMetaData.SourcePath.lexically_relative(PathToAssetFolder).string();
		out << YAML::Key << "SourcePath" << YAML::Value << std::filesystem::relative(assetMetaData.SourcePath, PathToAssetFolder).string();
		out << YAML::Key << "CachePath" << YAML::Value << std::filesystem::relative(assetMetaData.CachePath, PathToAssetFolder).string();
		out << YAML::Key << "LastModifiedDate" << YAML::Value << assetMetaData.importDate;
		out << YAML::EndMap;
	}

	AssetMetaData DeserializeMetaFile(YAML::Node& node, std::filesystem::path PathToAssetFolder)
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

		metaData.SourcePath = PathToAssetFolder.parent_path() / str;

		str = node["CachePath"].as<std::string>();

		pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		metaData.CachePath = PathToAssetFolder.parent_path() / str;
		metaData.importDate = node["LastModifiedDate"].as<uint64_t>();
		
		return metaData;
	}

	AssetMetaData MetaFileSerializer::GetAssetMetaDataFile(std::filesystem::path const& path)
	{
		std::ifstream metaFile(path);

		if (!metaFile.is_open()) {
			BOREALIS_CORE_ASSERT(false, "Failed to open meta file");
			return {};
		}

		std::stringstream metaStream;
		metaStream << metaFile.rdbuf();
		metaFile.close();

		YAML::Node metaRoot = YAML::Load(metaStream.str());

		return DeserializeMetaFile(metaRoot, PathToAssetFolder);
	}

	AssetMetaData MetaFileSerializer::CreateAssetMetaFile(std::filesystem::path const& path)
	{
		AssetMetaData metaData = GetAssetMetaData(path);

		std::filesystem::path metaFilePath;

		if (std::filesystem::is_directory(path))
		{
			metaFilePath = path.string() + ".meta";
		}
		else
		{
			metaFilePath = path;
			metaFilePath.replace_extension(".meta");
		}
		
		YAML::Emitter out;
		SerializeMetaFile(out, metaData, PathToAssetFolder);

		SaveAsFile(metaFilePath, out.c_str());

		return metaData;
	}

	AssetMetaData MetaFileSerializer::CreateAssetMetaFile(std::filesystem::path const& path, AssetHandle handle)
	{
		AssetMetaData metaData = GetAssetMetaData(path);

		metaData.Handle = handle;

		std::filesystem::path metaFilePath;

		if (std::filesystem::is_directory(path))
		{
			metaFilePath = path.string() + ".meta";
		}
		else
		{
			metaFilePath = path;
			metaFilePath.replace_extension(".meta");
		}

		YAML::Emitter out;
		SerializeMetaFile(out, metaData, PathToAssetFolder);

		SaveAsFile(metaFilePath, out.c_str());

		return metaData;
	}

	void MetaFileSerializer::SerialzeRegistry(std::filesystem::path assetRegistryPath, std::unordered_map<AssetHandle, AssetMetaData> const& assetRegistry)
	{
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		for (auto metaData : assetRegistry)
		{
			SerializeMetaFile(out, metaData.second, PathToAssetFolder);
		}

		out << YAML::EndSeq
			<< YAML::EndMap;

		SaveAsFile(assetRegistryPath, out.c_str());
	}

	void MetaFileSerializer::DeserializeRegistry(std::string const& registryFileString, std::unordered_map<AssetHandle, AssetMetaData>& AssetRegistry)
	{
		YAML::Node registryRoot = YAML::Load(registryFileString);

		YAML::Node assetMetaInfo = registryRoot["Assets"];

		if (assetMetaInfo)
		{
			std::filesystem::path pathToAssetFolder = PathToAssetFolder;
			for (YAML::Node metaInfo : assetMetaInfo)
			{
				AssetMetaData metaData = DeserializeMetaFile(metaInfo, pathToAssetFolder);

				AssetRegistry.insert({ metaData.Handle, metaData });
			}
		}
	}

	void MetaFileSerializer::SaveAsFile(const std::filesystem::path& path, const char* outputFile)
	{
		std::ofstream outStream(path);
		outStream << outputFile;
		outStream.close();
	}

	std::time_t MetaFileSerializer::GetLastWriteTime(const std::filesystem::path& path)
	{
		auto ftime = std::filesystem::last_write_time(path);

		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			ftime - std::filesystem::file_time_type::clock::now() +
			std::chrono::system_clock::now());

		return std::chrono::system_clock::to_time_t(sctp);
	}

	AssetMetaData MetaFileSerializer::GetAssetMetaData(std::filesystem::path const& path)
	{
		AssetMetaData metaData;

		metaData.name = path.filename().string();
		metaData.Handle = UUID();

		metaData.Type = Asset::GetAssetTypeFromExtention(path);

		metaData.SourcePath = path;// .lexically_relative(PathToAssetFolder);

		metaData.importDate = GetLastWriteTime(path);

		return metaData;
	}
}

