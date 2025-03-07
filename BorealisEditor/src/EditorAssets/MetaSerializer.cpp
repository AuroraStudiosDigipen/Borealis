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
#include <Assets/AssetManager.hpp>
#include <Core/LoggerSystem.hpp>
#include <EditorAssets/MetaSerializer.hpp>

#include <yaml-cpp/yaml.h>
#include <thread>
#include <assimp/zlib.h>

namespace Borealis
{
	void MetaFileSerializer::SetAssetFolderPath(std::filesystem::path const& path)
	{
		PathToAssetFolder = path;
	}

	std::string GetTextureTypeString(TextureType type)
	{
		switch (type)
		{
		case Borealis::TextureType::_DEFAULT:
			return "Default";
		case Borealis::TextureType::_NORMAL_MAP:
			return "Normal Map";
		default:
			return "Invalid";
		}
		return {};
	}

	std::string GetTextureShapeString(TextureShape type)
	{
		switch (type)
		{
		case Borealis::TextureShape::_2D:
			return "2D";
		case Borealis::TextureShape::_CUBE:
			return "Cube";
		default:
			return "Invalid";
		}
		return {};
	}

	std::string GetTextureWrapString(TextureWrap type)
	{
		switch (type)
		{
		case Borealis::TextureWrap::_REPEAT:
			return "Repeat";
		case Borealis::TextureWrap::_MIRRORED:
			return "Mirrored";
		case Borealis::TextureWrap::_CLAMP_TO_EDGE:
			return "ClampToEdge";
		case Borealis::TextureWrap::_CLAMP_TO_BORDER:
			return "ClampToBorder";
		default:
			return "Invalid";
		}
	}

	std::string GetTextureFilterString(TextureFilter type)
	{
		switch (type)
		{
		case Borealis::TextureFilter::_LINEAR:
			return "Linear";
		case Borealis::TextureFilter::_NEAREST:
			return "Nearest";
		default:
			break;
		}
	}

	void SerializeTextureConfig(YAML::Emitter& out, TextureConfig const& textureConfig)
	{
		out << YAML::Key << "TextureType" << YAML::Value << GetTextureTypeString(textureConfig.type);
		out << YAML::Key << "TextureShape" << YAML::Value << GetTextureShapeString(textureConfig.shape);
		out << YAML::Key << "sRGB" << YAML::Value << textureConfig.sRGB;
		out << YAML::Key << "MipMaps" << YAML::Value << textureConfig.generateMipMaps;
		out << YAML::Key << "TextureWrap" << YAML::Value << GetTextureWrapString(textureConfig.wrapMode);
		//out << YAML::Key << "TextureFilter" << YAML::Value << GetTextureFilterString(textureConfig.filterMode);
		out << YAML::Key << "TextureFilter" << YAML::Value << GetTextureFilterString(TextureFilter::_NEAREST);
	}

	void SerializeMeshConfig(YAML::Emitter& out, MeshConfig const& meshConfig)
	{
		out << YAML::Key << "IsSkinnedMesh" << YAML::Value << meshConfig.skinMesh;
	}

	void SerializeMetaConfigFile(YAML::Emitter& out, AssetType type, AssetConfig const& assetConfig)
	{
		switch (type)
		{
		case Borealis::AssetType::None:
			break;
		case Borealis::AssetType::Audio:
			break;
		case Borealis::AssetType::Mesh:
			SerializeMeshConfig(out, GetConfig<MeshConfig>(assetConfig));
			break;
		case Borealis::AssetType::Shader:
			break;
		case Borealis::AssetType::Texture2D:
			SerializeTextureConfig(out, GetConfig<TextureConfig>(assetConfig));
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
	}

	void SerializeMetaFile(YAML::Emitter& out, AssetMetaData const& assetMetaData, std::filesystem::path PathToAssetFolder)
	{
		std::filesystem::path assetFolderPath;
		out << YAML::BeginMap;
		out << YAML::Key << "Version" << YAML::Value << META_VERSION;
		out << YAML::Key << "Name" << YAML::Value << assetMetaData.name;
		out << YAML::Key << "AssetHandle" << YAML::Value << assetMetaData.Handle;
		out << YAML::Key << "AssetType" << YAML::Value << AssetManager::AssetTypeToString(assetMetaData.Type);
		SerializeMetaConfigFile(out, assetMetaData.Type ,assetMetaData.Config);
		out << YAML::Key << "SourcePath" << YAML::Value << std::filesystem::relative(assetMetaData.SourcePath, PathToAssetFolder).string();
		out << YAML::Key << "CachePath" << YAML::Value << std::filesystem::relative(assetMetaData.CachePath, PathToAssetFolder).string();
		out << YAML::Key << "SourceFileHash" << YAML::Value << assetMetaData.SourceFileHash;
		out << YAML::EndMap;
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

	TextureWrap GetTextureWrap(std::string const& typeStr)
	{
		if (typeStr == "Repeat") return TextureWrap::_REPEAT;
		if (typeStr == "Mirrored") return TextureWrap::_MIRRORED;
		if (typeStr == "ClampToEdge") return TextureWrap::_CLAMP_TO_EDGE;
		if (typeStr == "ClampToBorder") return TextureWrap::_CLAMP_TO_BORDER;
		return TextureWrap::_REPEAT;
	}

	TextureFilter GetTextureFilter(std::string const& typeStr)
	{
		if (typeStr == "Linear") return TextureFilter::_LINEAR;
		if (typeStr == "Nearest") return TextureFilter::_NEAREST;
		return TextureFilter::_LINEAR;
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
		if (node["TextureWrap"])
			config.wrapMode = GetTextureWrap(node["TextureWrap"].as<std::string>());
		if (node["TextureFilter"])
			config.wrapMode = GetTextureWrap(node["TextureFilter"].as<std::string>());
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
		case Borealis::AssetType::Audio:
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

	void DeserializeVersionlessMeta(AssetMetaData& metaData, YAML::Node& node, std::filesystem::path const& PathToAssetFolder)
	{
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

		metaData.SourcePath = PathToAssetFolder.parent_path() / str;

		str = node["CachePath"].as<std::string>();

		pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		metaData.CachePath = PathToAssetFolder.parent_path() / str;
		//no longer used
		//metaData.importDate = node["LastModifiedDate"].as<uint64_t>();
	}

	void DeserializeVersionMeta0_01(AssetMetaData& metaData, YAML::Node& node, std::filesystem::path const& PathToAssetFolder)
	{
		metaData.Version = node["Version"].as<double>();
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

		metaData.SourcePath = PathToAssetFolder.parent_path() / str;

		str = node["CachePath"].as<std::string>();

		pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		metaData.CachePath = PathToAssetFolder.parent_path() / str;

		metaData.SourceFileHash = node["SourceFileHash"].as<uint32_t>();
	}

	AssetMetaData DeserializeMetaFile(YAML::Node& node, std::filesystem::path PathToAssetFolder)
	{
		AssetMetaData metaData;

		if (node["Version"])
		{
			double version = node["Version"].as<double>();

			if (version == 0.01)
			{
				DeserializeVersionMeta0_01(metaData, node, PathToAssetFolder);
			}
		}
		else
		{
			DeserializeVersionlessMeta(metaData, node, PathToAssetFolder);
		}

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

		// Check if asset registry exists
		AssetMetaData metaData = GetAssetMetaData(path);

		std::filesystem::path metaFilePath = path.string() + ".meta";
		
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
			metaFilePath = path.string() + ".meta";
		}

		YAML::Emitter out;
		SerializeMetaFile(out, metaData, PathToAssetFolder);

		SaveAsFile(metaFilePath, out.c_str());

		return metaData;
	}

	void MetaFileSerializer::SaveMetaFile(AssetMetaData const& metaData)
	{
		YAML::Emitter out;
		std::filesystem::path path = metaData.SourcePath;

		while (!path.empty())
		{
			if (path.filename() == "Assets")
			{
				break; // Found the Assets folder
			}
			path = path.parent_path(); // Go up one level
		}

		path = path.parent_path();

		SerializeMetaFile(out, metaData, path);

		std::filesystem::path metaFilePath = metaData.SourcePath.string() + ".meta";
		SaveAsFile(metaFilePath, out.c_str());
	}

	void MetaFileSerializer::SerialzeRegistry(std::filesystem::path const& assetRegistryPath, std::unordered_map<AssetHandle, AssetMetaData> const& assetRegistry)
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

	void MetaFileSerializer::DeserializeRegistry(std::string const& registryFileString, AssetRegistry& AssetRegistry, AssetRegistrySrcLoc& RegistrySrcLoc)
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
				RegistrySrcLoc.insert({ metaData.SourcePath.string(), metaData.Handle });
			}
		}
	}

	uint32_t MetaFileSerializer::HashFile(std::filesystem::path const& path)
	{
		if (std::filesystem::is_directory(path)) return 0;

		const int maxRetries = 5;
		const std::chrono::milliseconds retryDelay(50);

		for (int attempt = 0; attempt < maxRetries; ++attempt) {
			std::ifstream file(path, std::ios::binary);
			if (file.is_open()) {
				std::vector<char> buffer(4096);
				uint32_t crc = 0;

				while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
					crc = crc32(crc, reinterpret_cast<const unsigned char*>(buffer.data()), file.gcount());
				}

				file.close();
				return crc;
			}

			// Wait and retry
			std::this_thread::sleep_for(retryDelay);
		}

		return 0;
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

		metaData.Type = AssetManager::GetAssetTypeFromExtension(path);

		metaData.Config = GetDefaultConfig(metaData.Type);

		metaData.SourcePath = path;

		metaData.SourceFileHash = HashFile(path);

		return metaData;
	}

	AssetConfig MetaFileSerializer::GetDefaultConfig(AssetType type)
	{
		AssetConfig assetConfig{};

		switch (type)
		{
		case Borealis::AssetType::None:
			break;
		case Borealis::AssetType::Audio:
			break;
		case Borealis::AssetType::Mesh:
			assetConfig = MeshConfig{};
			break;
		case Borealis::AssetType::Shader:
			break;
		case Borealis::AssetType::Texture2D:
			assetConfig = TextureConfig{};
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

		return assetConfig;
	}
}

