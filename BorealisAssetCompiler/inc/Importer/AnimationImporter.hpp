/******************************************************************************
/*!
\file       AnimationImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 04, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AnimationImporter_HPP
#define AnimationImporter_HPP

#include <filesystem>
#include <string>

#include "Importer/AssetStructs.hpp"

namespace BorealisAssetCompiler
{
	class AnimationImporter
	{
	public:
		static void LoadAnimations(Animation& animation, std::string const& animationPath);
		static void SaveAnimation(Animation const& anim, std::filesystem::path const& path);
	};
}

#endif
