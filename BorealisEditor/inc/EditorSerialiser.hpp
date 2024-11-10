/******************************************************************************
/*!
\file       EditorSerialiser.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef EditorSerialiser_HPP
#define EditorSerialiser_HPP
#include <Scene/Serialiser.hpp>
namespace Borealis
{

	class EditorSerialiser : public Serialiser
	{
	public:
		EditorSerialiser(const Ref<Scene>& scene) : Serialiser(scene) {}
		void SerialiseAbstractItems(YAML::Emitter& out, Entity& entity) override;
		void DeserialiseAbstractItems(YAML::detail::iterator_value& data, Entity& entity) override;
	};
}

#endif
