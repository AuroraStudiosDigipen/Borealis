/******************************************************************************/
/*!
\file		Serialiser.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 14, 2024
\brief		Declares the class for Serialiser

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SERIALISER_HPP
#define SERIALISER_HPP
#include <Core/Core.hpp>
#include <Scene/Scene.hpp>

namespace YAML
{
	class Emitter;
	class Node;
	namespace detail
	{
		struct iterator_value;
	}
}
namespace Borealis
{
	class BehaviourNode;
	class BehaviourTree;
	class Serialiser
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor of the serialiser
			\param[in] scene
				Reference to the scene to be serialised
		*************************************************************************/
		Serialiser(const Ref<Scene>& scene);
		
		/*!***********************************************************************
			\brief
				Serialises the scene to a YAML file
			\param[in] filepath
				The path to the file to be serialised
			\return
				True if the serialisation was successful, false otherwise
		*************************************************************************/
		bool SerialiseScene(const std::string& filepath);

		/*!***********************************************************************
			\brief
				
				s the scene from a YAML file
			\param[in] filepath
				The path to the file to be deserialised
			\return
				True if the deserialisation was successful, false otherwise
		*************************************************************************/
		bool DeserialiseScene(const std::string& filepath);

		virtual void SerialiseAbstractItems(YAML::Emitter& out, Entity& entity) {};
		virtual void DeserialiseAbstractItems(YAML::detail::iterator_value& data, Entity& entity) {};

		virtual void operator() (const Ref<Scene>& scene);

		void SerialisePrefab(const std::string& filepath, Entity entity);

		/*!***********************************************************************
			\brief
				Serialises the editor style data
			\return
				True if the serialisation was successful, false otherwise
		*************************************************************************/
		static bool SerialiseEditorStyle();

		/*!***********************************************************************
			\brief
				Deserialises the editor style data
			\return
				True if the deserialisation was successful, false otherwise
		*************************************************************************/
		static bool DeserialiseEditorStyle();

		/*!***********************************************************************
			\brief
				Serialises the behaviour tree to a YAML file
			\param[in] filepath
				The path to the file to be serialised
			\param[in] tree
				The behaviour tree to be serialised
			\return
				True if the serialisation was successful, false otherwise
		*************************************************************************/
		static bool SerializeBehaviourNode(YAML::Emitter& out, const Ref<BehaviourNode> node);

		entt::entity DeserialiseEntity(YAML::detail::iterator_value& node, entt::registry& registry, UUID& uuid);

	private:

		void SerializeEntity(YAML::Emitter& out, Entity& entity);
		/*!***********************************************************************
			\brief
				Recursively parses the YAML node to construct the behavior tree.
			\param[in] node
				The current YAML node to parse.
			\param[in] parentNode
				The parent node in the behavior tree to which the parsed node will be added.
			\param[in] tree
				The behaviour tree object to which nodes are being added.
			\param[in] parentDepth
				The depth of the parent node in the behavior tree.
		*************************************************************************/
		void ParseTree(YAML::Node& node, Ref<BehaviourNode> parentNode, BehaviourTree& tree, int parentDepth);

		Ref<Scene> mScene;

	};
}

#endif
