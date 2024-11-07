/******************************************************************************
/*!
\file       EditorSerialiser.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <EditorSerialiser.hpp>
#include <Scene/Entity.hpp>
#include <PrefabComponent.hpp>
#include <Scene/SerialiserUtils.hpp>

namespace Borealis
{
	void EditorSerialiser::SerialiseAbstractItems(YAML::Emitter& out, Entity& entity)
	{
		if (entity.HasComponent<PrefabComponent>())
		{
			SerializeComponent(out, entity.GetComponent<PrefabComponent>());
		}
	}
}

