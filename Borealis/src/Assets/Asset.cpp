/******************************************************************************
/*!
\file       Asset.cpp
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
#include <Assets/Asset.hpp>

namespace Borealis
{
	AssetType Asset::GetAssetTypeFromExtention(std::filesystem::path path)
	{
		if (std::filesystem::is_directory(path))
			return AssetType::Folder;

		std::string extension = path.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".png") return AssetType::Texture2D;
		if (extension == ".sc") return AssetType::Scene;
		if (extension == ".glsl") return AssetType::Scene;

		return AssetType::None;
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
		if (type == "AssetType::Scene")			return AssetType::Scene;
	}
}

