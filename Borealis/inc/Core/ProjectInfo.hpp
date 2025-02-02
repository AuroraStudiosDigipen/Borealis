/******************************************************************************
/*!
\file       ProjectInfo.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 15, 2024
\brief      Declares the struct for project information

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ProjectInfo_HPP
#define ProjectInfo_HPP

#include <string>
#include <filesystem>

namespace Borealis
{
	struct ProjectInfo
	{
		std::string ProjectName; // Name of the project
		std::filesystem::path ProjectPath; // Path to the project
		std::filesystem::path AssetsPath; // Path to the assets folder
		std::filesystem::path CachePath; // Path to the assets folder
		std::filesystem::path AssetsRegistryPath; // Path to the asset registry file

		std::string AssetsDirectoryName = "\\Assets"; // Name of the assets folder
		std::string AssetsRegistryName = "\\AssetRegistry.brdb"; // Name of the asset registry file
	};
}

#endif
