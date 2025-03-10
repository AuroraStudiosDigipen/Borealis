/******************************************************************************
/*!
\file       MetaSerializer.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <iostream>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "MetaSerializer.hpp"

namespace BorealisAssetCompiler
{
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
			config.filterMode = GetTextureFilter(node["TextureFilter"].as<std::string>());
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
		case AssetType::None:
			break;
		case AssetType::Audio:
			break;
		case AssetType::Mesh:
			config = DeserializeMeshConfig(node);
			break;
		case AssetType::Shader:
			break;
		case AssetType::Texture2D:
			config = DeserializeTextureConfig(node);
			break;
		case AssetType::Folder:
			break;
		case AssetType::Font:
			break;
		case AssetType::Scene:
			break;
		case AssetType::Material:
			break;
		case AssetType::Prefab:
			break;
		default:
			break;
		}

		return config;
	}

	AssetMetaData DeserializeMetaFile(YAML::Node& node, std::filesystem::path PathToAssetFolder)
	{
		AssetMetaData metaData;

		metaData.Version = node["Version"].as<double>();
		metaData.name = node["Name"].as<std::string>();
		metaData.Handle = node["AssetHandle"].as<uint64_t>();
		metaData.Type = Asset::StringToAssetType(node["AssetType"].as<std::string>());
		metaData.Config = DeserializeMetaConfigFile(node, metaData.Type);

		std::string str = node["SourcePath"].as<std::string>();

		const std::string pattern = "..\\";
		size_t pos = str.find(pattern);

		// If the pattern is found, erase it
		if (pos != std::string::npos) {
			str.erase(pos, pattern.length());
		}

		metaData.SourcePath = PathToAssetFolder / str;
		metaData.SourceFileHash = node["SourceFileHash"].as<uint32_t>();

		return metaData;
	}

	std::string Asset::AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:		return "AssetType::None";
		case AssetType::Audio:		return "AssetType::Audio";
		case AssetType::Mesh:		return "AssetType::Mesh";
		case AssetType::Shader:		return "AssetType::Shader";
		case AssetType::Texture2D:	return "AssetType::Texture2D";
		case AssetType::Folder:		return "AssetType::Folder";
		case AssetType::Font:		return "AssetType::Font";
		case AssetType::Scene:		return "AssetType::Scene";
		}

		return "AssetType::<Invalid>";
	}

	AssetType Asset::StringToAssetType(std::string const& type)
	{
		if (type == "AssetType::None")			return AssetType::None;
		if (type == "AssetType::Audio")			return AssetType::Audio;
		if (type == "AssetType::Mesh")			return AssetType::Mesh;
		if (type == "AssetType::Shader")		return AssetType::Shader;
		if (type == "AssetType::Texture2D")		return AssetType::Texture2D;
		if (type == "AssetType::Folder")		return AssetType::Folder;
		if (type == "AssetType::Font")			return AssetType::Font;
		if (type == "AssetType::Scene")			return AssetType::Scene;

		return AssetType::None;
	}

	AssetMetaData MetaSerializer::GetAssetMetaDataFile(std::filesystem::path const& path)
	{
		std::ifstream metaFile(path);

		if (!metaFile.is_open()) {
			return {};
		}

		std::stringstream metaStream;
		metaStream << metaFile.rdbuf();
		metaFile.close();

		YAML::Node metaRoot = YAML::Load(metaStream.str());


		std::filesystem::path currentPath = path;
		if (path.string().find("Assets") != std::string::npos)
		{
			// Go up the directory structure until we reach "assets"
			while (!currentPath.empty() && currentPath.filename() != "Assets")
			{
				currentPath = currentPath.parent_path();
			}
		}

		return DeserializeMetaFile(metaRoot, currentPath.parent_path());
	}

	AssetConfig MetaSerializer::GetDefaultConfig(AssetType type)
	{
		AssetConfig assetConfig{};

		switch (type)
		{
		case AssetType::None:
			break;
		case AssetType::Audio:
			break;
		case AssetType::Mesh:
			assetConfig = MeshConfig{};
			break;
		case AssetType::Shader:
			break;
		case AssetType::Texture2D:
			assetConfig = TextureConfig{};
			break;
		case AssetType::Folder:
			break;
		case AssetType::Font:
			break;
		case AssetType::Scene:
			break;
		case AssetType::Material:
			break;
		case AssetType::Prefab:
			break;
		default:
			break;
		}

		return assetConfig;
	}

	std::string GetTextureTypeString(TextureType type)
	{
		switch (type)
		{
		case TextureType::_DEFAULT:
			return "Default";
		case TextureType::_NORMAL_MAP:
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
		case TextureShape::_2D:
			return "2D";
		case TextureShape::_CUBE:
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
		case TextureWrap::_REPEAT:
			return "Repeat";
		case TextureWrap::_MIRRORED:
			return "Mirrored";
		case TextureWrap::_CLAMP_TO_EDGE:
			return "ClampToEdge";
		case TextureWrap::_CLAMP_TO_BORDER:
			return "ClampToBorder";
		default:
			return "Invalid";
		}
	}

	std::string GetTextureFilterString(TextureFilter type)
	{
		switch (type)
		{
		case TextureFilter::_LINEAR:
			return "Linear";
		case TextureFilter::_NEAREST:
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
		out << YAML::Key << "TextureFilter" << YAML::Value << GetTextureFilterString(textureConfig.filterMode);
	}

	void SerializeMeshConfig(YAML::Emitter& out, MeshConfig const& meshConfig)
	{
		out << YAML::Key << "IsSkinnedMesh" << YAML::Value << meshConfig.skinMesh;
	}

	void SerializeMetaConfigFile(YAML::Emitter& out, AssetType type, AssetConfig const& assetConfig)
	{
		switch (type)
		{
		case AssetType::None:
			break;
		case AssetType::Audio:
			break;
		case AssetType::Mesh:
			SerializeMeshConfig(out, GetConfig<MeshConfig>(assetConfig));
			break;
		case AssetType::Shader:
			break;
		case AssetType::Texture2D:
			SerializeTextureConfig(out, GetConfig<TextureConfig>(assetConfig));
			break;
		case AssetType::Folder:
			break;
		case AssetType::Font:
			break;
		case AssetType::Scene:
			break;
		case AssetType::Material:
			break;
		case AssetType::Prefab:
			break;
		default:
			break;
		}
	}

	void SerializeMetaFile(YAML::Emitter& out, AssetMetaData const& assetMetaData, std::filesystem::path PathToAssetFolder)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Version" << YAML::Value << assetMetaData.Version;
		out << YAML::Key << "Name" << YAML::Value << assetMetaData.name;
		out << YAML::Key << "AssetHandle" << YAML::Value << assetMetaData.Handle;
		out << YAML::Key << "AssetType" << YAML::Value << Asset::AssetTypeToString(assetMetaData.Type);
		SerializeMetaConfigFile(out, assetMetaData.Type, assetMetaData.Config);
		//out << YAML::Key << "SourcePath" << YAML::Value << assetMetaData.SourcePath.lexically_relative(PathToAssetFolder).string();
		out << YAML::Key << "SourcePath" << YAML::Value << std::filesystem::relative(assetMetaData.SourcePath, PathToAssetFolder).string();
		out << YAML::Key << "CachePath" << YAML::Value << std::filesystem::relative(assetMetaData.CachePath, PathToAssetFolder).string();
		out << YAML::Key << "SourceFileHash" << YAML::Value << assetMetaData.SourceFileHash;
		out << YAML::EndMap;
	}

	void SaveAsFile(std::filesystem::path const& path, const char* outputFile)
	{
		std::ofstream outStream(path);
		outStream << outputFile;
		outStream.close();
	}

	void MetaSerializer::SaveMetaFile(AssetMetaData metaData, std::filesystem::path const& metaFilePath)
	{
		YAML::Emitter out;

		std::filesystem::path currentPath = metaData.SourcePath;
		if (metaData.SourcePath.string().find("Assets") != std::string::npos)
		{
			// Go up the directory structure until we reach "assets"
			while (!currentPath.empty() && currentPath.filename() != "Assets")
			{
				currentPath = currentPath.parent_path();
			}
		}

		SerializeMetaFile(out, metaData, currentPath.parent_path());

		SaveAsFile(metaFilePath, out.c_str());
	}
}

