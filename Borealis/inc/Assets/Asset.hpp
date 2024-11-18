/******************************************************************************
/*!
\file       Asset.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 23, 2024
\brief      Holds the asset class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef Asset_HPP
#define Asset_HPP

#include <filesystem>

#include <rttr/rttr_enable.h>

#include <Core/UUID.hpp>

namespace Borealis
{
	using AssetHandle = UUID;

	enum class AssetType
	{
		None,
		Animation,
		Audio,
		Mesh,
		Shader,
		Texture2D,
		Folder,
		Font,
		Scene,
		Material,
		Prefab,
		BehaviourTree,
		Script
	};

	class Asset
	{
	public:
		AssetHandle mAssetHandle{};
		AssetType mAssetType{AssetType::None};

		static AssetType GetAssetTypeFromExtention(std::filesystem::path path);
		static std::string AssetTypeToString(AssetType type);
		static AssetType StringToAssetType(std::string const& type);

		RTTR_ENABLE()
	};
}


#endif
