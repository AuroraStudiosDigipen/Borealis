/******************************************************************************
/*!
\file       AssetConfigs.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 2, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetConfigs_HPP
#define AssetConfigs_HPP

#include <variant>

namespace Borealis
{
	enum class TextureType
	{
		_2D,
		_CUBE
	};

	struct TextureConfig 
	{
		TextureType type = TextureType::_2D;
		bool sRGB = false;
		bool generateMipMaps = false;
	};

	struct MeshConfig
	{
		bool skinMesh = false;
	};

	struct FontConfig {};

	using AssetConfig = std::variant
		<
		TextureConfig,
		MeshConfig,
		FontConfig
		/*...*/
		>;

	template<typename T>
	T GetConfig(AssetConfig const& assetConfig)
	{
		if (std::holds_alternative<T>(assetConfig))
			return std::get<T>(assetConfig);


		//error check here
		return T{};
	}
}

#endif
