/******************************************************************************/
/*!
\file		Components.hpp
\author 	Yeo Jun Jie
\par    	email: junjie.yeo\@digipen.edu
\date   	Sept 12, 2024
\brief		Declares the component structs and classes

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef PREFABCOMPONENT_HPP
#define PREFABCOMPONENT_HPP
#include <Core/UUID.hpp>
#include <unordered_set>
#include <typeindex>
#include "entt.hpp"

namespace Borealis
{
	struct PrefabComponent
	{
		entt::entity mPrefabID;
		UUID mParentID;
		std::unordered_set<std::string> mEditedComponentList;

	};
}


#endif