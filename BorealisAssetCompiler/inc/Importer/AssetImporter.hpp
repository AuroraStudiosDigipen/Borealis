/******************************************************************************
/*!
\file       AssetImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetImporter_HPP
#define AssetImporter_HPP

#include "MetaSerializer.hpp"

namespace BorealisAssetCompiler
{
	class AssetImporter
	{
	public:

		static AssetMetaData ImportAsset(AssetMetaData metaData);
	};
}

#endif
