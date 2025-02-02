/******************************************************************************
/*!
\file       AssetImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "Importer/AssetImporter.hpp"
#include "Importer/TextureImporter.hpp"
#include "Importer/FontImporter.hpp"
#include "Importer/MeshImporter.hpp"

namespace BorealisAssetCompiler
{
	AssetMetaData AssetImporter::ImportAsset(AssetMetaData metaData)
	{
		std::filesystem::path cacheString = metaData.SourcePath;

		std::string toReplace = "Assets";
		std::string replacement = "Cache";

		while (cacheString.filename() != toReplace)
		{
			cacheString = cacheString.parent_path();
		}

		cacheString.replace_filename(replacement);

		std::filesystem::path cachePath = cacheString.append(std::to_string(metaData.Handle));

		std::cout << "In compiler, cache path : " << cachePath.string() << '\n';

		std::filesystem::path directoryPath = cachePath.parent_path();

		// Create the directories if they don't exist
		//redundant now
		if (!std::filesystem::exists(directoryPath))
		{
			if (!std::filesystem::create_directories(directoryPath))
			{
				//Error check
			}
		}

		switch (metaData.Type)
		{
		case AssetType::Texture2D:
			TextureImporter::SaveFile(metaData.SourcePath, metaData.Config, cachePath);
			break;
		case AssetType::Font:
			FontImporter::SaveFile(metaData.SourcePath, cachePath);
			break;
		case AssetType::Mesh:
			MeshImporter::SaveFile(metaData.SourcePath, metaData.Config, cachePath);
			break;
		case AssetType::None:
		default:
			break;
		}

		//no need cache path anymore
		metaData.CachePath = cachePath;
		return metaData;
	}
}

