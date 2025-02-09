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
		_DEFAULT,
		_NORMAL_MAP
	};

	enum class TextureShape
	{
		_2D,
		_CUBE
	};

	enum class TextureWrap
	{
		_REPEAT,
		_MIRRORED,
		_CLAMP_TO_EDGE,
		_CLAMP_TO_BORDER
	};

	enum class TextureFilter
	{
		_LINEAR,
		_NEAREST
	};

	struct TextureConfig 
	{
		TextureType type = TextureType::_DEFAULT;
		TextureShape shape = TextureShape::_2D;
		bool sRGB = true;
		bool generateMipMaps = false;
		TextureWrap wrapMode = TextureWrap::_REPEAT;
		TextureFilter filterMode = TextureFilter::_LINEAR;
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
