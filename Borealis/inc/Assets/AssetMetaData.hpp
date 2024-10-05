/******************************************************************************
/*!
\file       AssetMetaData.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 23, 2024
\brief      Holds struct for assets meta data

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetMetaData_HPP
#define AssetMetaData_HPP

#include <chrono>
#include <ctime>
#include <filesystem>

#include <Assets/Asset.hpp>

namespace Borealis
{
	struct AssetMetaData
	{
		std::string name{};

		AssetHandle Handle;
		AssetType Type = AssetType::None;

		std::filesystem::path CachePath{};
		std::filesystem::path SourcePath{};

		std::time_t importDate{};
		std::time_t cacheCreationDate{};
	};
}

#endif
