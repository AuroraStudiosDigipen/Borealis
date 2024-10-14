/******************************************************************************/
/*!
\file		Serialiser.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 14, 2024
\brief		Defines the class for Serialiser

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <yaml-cpp/yaml.h>
#include <imgui.h>
#include <Scene/ReflectionInstance.hpp>
#include <Scene/Serialiser.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <Core/LoggerSystem.hpp>
#include <ImGui/ImGuiFontLib.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <Assets/AssetManager.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Scripting/ScriptField.hpp>
#include <Scripting/ScriptingSystem.hpp>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};



	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<ImVec4>
	{
		static Node encode(const ImVec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, ImVec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}
namespace Borealis
{
	void Serialiser::ParseTree(YAML::Node& node, Ref<BehaviourNode> parentNode, BehaviourTree& tree, int parentDepth)
	{
		// Extract the node name and depth
		std::string nodeName = node["name"].as<std::string>();
		int depth = node["depth"].as<int>();

		// Create the node using NodeFactory based on its name
		Ref<BehaviourNode> currentNode = Borealis::NodeFactory::CreateNodeByName(nodeName);
		currentNode->SetDepth(depth); // Assuming setDepth is implemented in BehaviourNode

		// Add the current node to the tree
		tree.AddNode(parentNode, currentNode, depth);

		BOREALIS_CORE_TRACE("Deserialising node: {} at depth {}", nodeName, depth);

		// Process children if they exist
		if (node["children"])
		{
			for (auto childNode : node["children"])
			{
				// Recursively process each child node
				ParseTree(childNode, currentNode, tree, depth);
			}
		}
	}

	// Overrides:
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	void SerializeTexture(YAML::Emitter& out, Ref<Texture2D> texture)
	{
		if (texture)
		{
			out << YAML::Key << "Texture" << YAML::Value << texture->mAssetHandle;
		}
	}

	Serialiser::Serialiser(const Ref<Scene>& scene) : mScene(scene) {}


	bool Serialiser::SerializeBehaviourNode(YAML::Emitter& out, const Ref<BehaviourNode> node) {
		out << YAML::Key << "name" << YAML::Value << node->GetName();
		out << YAML::Key << "depth" << YAML::Value << node->GetDepth();
		if (!node->mChildren.empty())
		{
			out << YAML::Key << "children" << YAML::Value << YAML::BeginSeq;
			for (const auto& child : node->mChildren) {
				out << YAML::BeginMap;
				SerializeBehaviourNode(out, child); // Recursively serialize the child node
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		return true;
	}

	static bool SerializeProperty(YAML::Emitter& out, rttr::property& prop, const rttr::instance& instance)
	{
		auto propType = prop.get_type();
		auto propName = prop.get_name();
		auto propValue = prop.get_value(instance);
		out << YAML::Key << propName.to_string() << YAML::Value;

		if (propType.is_enumeration())
		{
			out << propType.get_enumeration().value_to_name(propValue).to_string();
			return true;
		}

		if (propType == rttr::type::get<int>())
		{
			out << propValue.to_int();
			return true;
		}

		if (propType == rttr::type::get<float>())
		{
			out << propValue.to_float();
			return true;
		}

		if (propType == rttr::type::get<bool>())
		{
			out << propValue.to_bool();
			return true;
		}

		if (propType == rttr::type::get<std::string>())
		{
			out << propValue.to_string();
			return true;
		}

		if (propType == rttr::type::get<unsigned char>())
		{
			out << (unsigned)propValue.to_uint8();
			return true;
		}

		if (propType == rttr::type::get<char>())
		{
			out << (int)propValue.to_int8();
			return true;
		}

		if (propType == rttr::type::get<unsigned short>())
		{
			out << (unsigned)propValue.to_uint16();
			return true;
		}

		if (propType == rttr::type::get<short>())
		{
			out << (int)propValue.to_int16();
			return true;
		}

		if (propType == rttr::type::get<unsigned>())
		{
			out << propValue.to_uint32();
			return true;
		}

		if (propType == rttr::type::get<long long>())
		{
			out << propValue.to_int64();
			return true;
		}

		if (propType == rttr::type::get<unsigned long long>())
		{
			out << propValue.to_uint64();
			return true;
		}


		if (propType == rttr::type::get<double>())
		{
			out << propValue.to_double();
			return true;
		}

		if (propType == rttr::type::get<glm::vec2>())
		{
			out << propValue.get_value<glm::vec2>();
			return true;
		}

		if (propType == rttr::type::get<glm::vec3>())
		{
			out << propValue.get_value<glm::vec3>();
			return true;
		}

		if (propType == rttr::type::get<glm::vec4>())
		{
			out << propValue.get_value<glm::vec4>();
			return true;
		}

		if (propType == rttr::type::get<Ref<Model>>())
		{
			out << propValue.get_value<Ref<Model>>()->mAssetHandle;
			return true;
		}

		if (propType == rttr::type::get<Ref<Material>>())
		{
			out << propValue.get_value<Ref<Material>>()->mAssetHandle;
			return true;
		}

		return false;
	}

	template <typename Component>
	static void SerializeComponent(YAML::Emitter& out, Component& component)
	{
		ReflectionInstance rInstance(component);
		out << YAML::Key << rInstance.get_type().get_name().to_string();
		out << YAML::BeginMap;
		auto properties = rInstance.get_type().get_properties();
		for (auto prop : properties)
		{
			SerializeProperty(out, prop, rInstance);
		}
		out << YAML::EndMap;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID();
		if (entity.HasComponent<TagComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TagComponent>());
		}

		if (entity.HasComponent<CameraComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CameraComponent>());
		}

		if (entity.HasComponent<TransformComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TransformComponent>());
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<SpriteRendererComponent>());
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CircleRendererComponent>());
		}

		if (entity.HasComponent<MeshFilterComponent>())
		{
			SerializeComponent(out, entity.GetComponent<MeshFilterComponent>());
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<MeshRendererComponent>());
		}

		if (entity.HasComponent<BoxColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<BoxColliderComponent>());
		}

		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CapsuleColliderComponent>());
		}

		if (entity.HasComponent<RigidBodyComponent>())
		{
			SerializeComponent(out, entity.GetComponent<RigidBodyComponent>());
		}

		if (entity.HasComponent<LightComponent>())
		{
			SerializeComponent(out, entity.GetComponent<LightComponent>());
		}

		if (entity.HasComponent<TextComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TextComponent>());
		}

		if (entity.HasComponent<AudioListenerComponent>())
		{
			SerializeComponent(out, entity.GetComponent<AudioListenerComponent>());
		}

		if (entity.HasComponent<AudioSourceComponent>())
		{
			SerializeComponent(out, entity.GetComponent<AudioSourceComponent>());
		}


		if (entity.HasComponent<BehaviourTreeComponent>())
		{
			out << YAML::Key << "BehaviourTreeComponent";
			out << YAML::BeginMap;

			auto& behaviourTreeComponent = entity.GetComponent<BehaviourTreeComponent>();
			
			for (auto& tree : behaviourTreeComponent.mBehaviourTrees)
			{
				out << YAML::Key << "BehaviourTree";
				out << YAML::BeginMap;
				out << YAML::Key << "Tree Name" << YAML::Value << tree->GetBehaviourTreeName();
				Serialiser::SerializeBehaviourNode(out, tree->GetRootNode());
				out << YAML::EndMap;
			}

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			for (auto[name, script]: scriptComponent.mScripts)
			{
				out << YAML::Key << name;
				out << YAML::BeginMap;

				for (auto [name,field] : script->GetScriptClass()->mFields)
				{
					if (field.mType == ScriptFieldType::Bool)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Bool";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<bool>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Float)
					{
						out <<  YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Float";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<float>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Int)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Int";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<int>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::String)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "String";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<std::string>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector2)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector2";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec2>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector3)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector3";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec3>(name);
						out << YAML::EndMap;
						continue;
					}

					if (field.mType == ScriptFieldType::Vector4)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector4";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec4>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UChar)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UChar";
						out << YAML::Key << "Data" << YAML::Value << static_cast<unsigned>(script->GetFieldValue<unsigned char>(name));
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Char)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Char";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<char>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UShort)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UShort";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned short>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Short)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Short";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<short>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UInt)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UInt";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Long)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Long";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<long long>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::ULong)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "ULong";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned long long>(name);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Double)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Double";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<double>(name);
						out << YAML::EndMap;
						continue;
					}
				}

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
		}


		out << YAML::EndMap;
	}

	bool Serialiser::SerialiseScene(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Scene" << YAML::Value << mScene->GetName()
		    << YAML::Key <<"Entities"	<<	YAML::Value << YAML::BeginSeq;

		auto view = mScene->mRegistry.view<entt::entity>();
		for (auto enttEntity : view)
		{
			Entity entity { enttEntity, mScene.get() };
			if (!entity)
			{
				return false;
			}
			SerializeEntity(out, entity);

		}
		out << YAML::EndSeq
			<< YAML::EndMap;

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
		return true;
	}

	bool Serialiser::DeserialiseScene(const std::string& filepath)
	{
		std::ifstream inStream(filepath);
		std::stringstream ss;
		ss << inStream.rdbuf();
		inStream.close();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Scene"])
		{
			BOREALIS_CORE_ERROR("Scene not found in file: {}", filepath);
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		BOREALIS_CORE_INFO("Deserialising scene: {}", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["EntityID"].as<uint64_t>(); // UUID

				auto tagComponent = entity["TagComponent"];
				std::string name;
				if (tagComponent)
				{
					name = tagComponent["Tag"].as<std::string>();
				}

				Entity loadedEntity = mScene->CreateEntityWithUUID(name, uuid);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = loadedEntity.GetComponent<TransformComponent>();
					tc.Translate = entity["TransformComponent"]["Translate"].as<glm::vec3>();
					tc.Rotation = entity["TransformComponent"]["Rotation"].as<glm::vec3>();
					tc.Scale = entity["TransformComponent"]["Scale"].as<glm::vec3>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = loadedEntity.AddComponent<SpriteRendererComponent>();
					src.Colour = entity["SpriteRendererComponent"]["Colour"].as<glm::vec4>();
					if (entity["SpriteRendererComponent"]["Texture"].IsDefined())
					{
						src.Texture = AssetManager::GetAsset<Texture2D>(entity["SpriteRendererComponent"]["Texture"].as<uint64_t>());
					}
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& src = loadedEntity.AddComponent<CircleRendererComponent>();
					src.Colour = entity["CircleRendererComponent"]["Colour"].as<glm::vec4>();
					src.thickness = entity["CircleRendererComponent"]["Thickness"].as<float>();
					src.fade = entity["CircleRendererComponent"]["Fade"].as<float>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = loadedEntity.AddComponent<CameraComponent>();
					cc.Camera.SetViewportSize(1920, 1080);
					cc.Camera.SetCameraType((SceneCamera::CameraType)cameraComponent["Camera"]["CameraType"].as<int>());
					cc.Camera.SetOrthoSize(cameraComponent["Camera"]["OrthoSize"].as<float>());
					cc.Camera.SetOrthoNear(cameraComponent["Camera"]["OrthoNear"].as<float>());
					cc.Camera.SetOrthoFar(cameraComponent["Camera"]["OrthoFar"].as<float>());
					cc.Camera.SetPerspFOV(cameraComponent["Camera"]["PerspFOV"].as<float>());
					cc.Camera.SetPerspNear(cameraComponent["Camera"]["PerspNear"].as<float>());
					cc.Camera.SetPerspFar(cameraComponent["Camera"]["PerspFar"].as<float>());
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

				}

				auto meshFilterComponent = entity["MeshFilterComponent"];
				if (meshFilterComponent)
				{
					auto& mfc = loadedEntity.AddComponent<MeshFilterComponent>();
					uint64_t uuid = entity["MeshFilterComponent"]["Mesh"].as<uint64_t>(); // UUID
					mfc.Model = AssetManager::GetAsset<Model>(uuid); // TODO: Load Mesh via UUID
					BOREALIS_CORE_INFO(mfc.Model->mAssetHandle);
				}

				auto meshRendererComponent = entity["MeshRendererComponent"];
				if (meshRendererComponent)
				{
					auto& mrc = loadedEntity.AddComponent<MeshRendererComponent>();
					uint64_t uuid = entity["MeshRendererComponent"]["Material"].as<uint64_t>();
					mrc.Material = AssetManager::GetAsset<Material>(uuid);
					mrc.castShadow = meshRendererComponent["CastShadow"].as<bool>();
				}

				auto boxColliderComponent = entity["BoxColliderComponent"];
				if (boxColliderComponent)
				{
					auto& bcc = loadedEntity.AddComponent<BoxColliderComponent>();
					bcc.isTrigger = boxColliderComponent["isTrigger"].as<bool>();
					bcc.providesContact = boxColliderComponent["providesContact"].as<bool>();
					bcc.Material = nullptr; // TODO: Load Material via UUID
					bcc.Center = boxColliderComponent["Center"].as<glm::vec3>();
					bcc.Size = boxColliderComponent["Size"].as<glm::vec3>();
				}

				auto capsuleColliderComponent = entity["CapsuleColliderComponent"];
				if (capsuleColliderComponent)
				{
					auto& ccc = loadedEntity.AddComponent<CapsuleColliderComponent>();
					ccc.isTrigger = capsuleColliderComponent["isTrigger"].as<bool>();
					ccc.providesContact = capsuleColliderComponent["providesContact"].as<bool>();
					ccc.Material = nullptr; // TODO: Load Material via UUID
					ccc.radius = capsuleColliderComponent["Radius"].as<float>();
					ccc.height = capsuleColliderComponent["Height"].as<float>();
					ccc.direction = (CapsuleColliderComponent::Direction)capsuleColliderComponent["Direction"].as<int>();
				}

				auto rigidBodyComponent = entity["RigidBodyComponent"];
				if (rigidBodyComponent)
				{
					auto& rbc = loadedEntity.AddComponent<RigidBodyComponent>();
					rbc.isBox = (RigidBodyType)rigidBodyComponent["isBox"].as<int>();
					rbc.radius = rigidBodyComponent["Radius"].as<float>();
					// run the update of the shape here once


					/*rbc.mass = rigidBodyComponent["mass"].as<float>();
					rbc.drag = rigidBodyComponent["drag"].as<float>();
					rbc.angularDrag = rigidBodyComponent["angularDrag"].as<float>();
					rbc.centerOfMass = rigidBodyComponent["centerOfMass"].as<glm::vec3>();
					rbc.inertiaTensor = rigidBodyComponent["inertiaTensor"].as<glm::vec3>();
					rbc.inertiaTensorRotation = rigidBodyComponent["inertiaTensorRotation"].as<glm::vec3>();
					rbc.AutomaticCenterOfMass = rigidBodyComponent["AutomaticCenterOfMass"].as<bool>();
					rbc.AutomaticTensor = rigidBodyComponent["AutomaticTensor"].as<bool>();
					rbc.useGravity = rigidBodyComponent["useGravity"].as<bool>();
					rbc.isKinematic = rigidBodyComponent["isKinematic"].as<bool>();*/
				}

				auto lightComponent = entity["LightComponent"];
				if (lightComponent)
				{
					auto& lc = loadedEntity.AddComponent<LightComponent>();

					lc.ambient = lightComponent["Ambient"].as<glm::vec3>();
					lc.diffuse = lightComponent["Diffuse"].as<glm::vec3>();
					lc.direction = lightComponent["Direction"].as<glm::vec3>();
					lc.specular = lightComponent["Specular"].as<glm::vec3>();
					lc.linear = lightComponent["Linear"].as<float>();
					lc.quadratic = lightComponent["Quadratic"].as<float>();
					lc.type = (LightComponent::Type)lightComponent["Type"].as<int>();
					lc.InnerOuterSpot.x = lightComponent["InnerSpotX"].as<float>();
					lc.InnerOuterSpot.y = lightComponent["InnerSpotY"].as<float>();

					/*lc.Colour = lightComponent["Colour"].as<glm::vec4>();
					lc.InnerOuterSpot = glm::vec2(lightComponent["InnerSpot"].as<float>(), lightComponent["OuterSpot"].as<float>());
					lc.Temperature = lightComponent["Temperature"].as<float>();
					lc.Intensity = lightComponent["Intensity"].as<float>();
					lc.IndirectMultiplier = lightComponent["IndirectMultiplier"].as<float>();
					lc.Range = lightComponent["Range"].as<float>();
					lc.type = (LightComponent::Type)lightComponent["Type"].as<int>();
					lc.shadowType = (LightComponent::ShadowType)lightComponent["ShadowType"].as<int>();
					lc.lightAppearance = (LightComponent::LightAppearance)lightComponent["LightAppearance"].as<int>();*/
				}
				auto behaviourTreeComponent = entity["BehaviourTreeComponent"];
				/*
					extract the name of tree and root node, then iteritivly build the tree, then call the clone method by createfromname function
					behaviourNode["name"]
				*/
				if (behaviourTreeComponent) 
				{
					//BOREALIS_CORE_TRACE("Parsed YAML: {}", behaviourTreeComponent);//used for debugging to see what is being read
					auto& btc = loadedEntity.AddComponent<BehaviourTreeComponent>();
					Ref<BehaviourTree> tempTree = MakeRef<BehaviourTree>();

					// Access the BehaviourTree node first
					auto behaviourTree = behaviourTreeComponent["BehaviourTree"];

					// Get the root node name and depth
					std::string treeName = behaviourTree["Tree Name"].as<std::string>();
					tempTree->SetBehaviourTreeName(treeName);
					std::string rootName = behaviourTree["name"].as<std::string>();
					int rootDepth = behaviourTree["depth"].as<int>();

					// Create root node using NodeFactory
					Ref<BehaviourNode> rootNode = Borealis::NodeFactory::CreateNodeByName(rootName);

					// Set the root node of the tree
					tempTree->SetRootNode(rootNode); //sets depth to 0 by default
					BOREALIS_CORE_TRACE("Deserialising BT {}", treeName);

					// If the root node has children, parse them recursively
					if (behaviourTree["children"]) {
						for (auto childNode : behaviourTree["children"]) {
							ParseTree(childNode, rootNode, *tempTree, rootDepth);
						}
					}
					btc.AddTree(tempTree);
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = loadedEntity.AddComponent<ScriptComponent>();
					for (const auto& script : scriptComponent)
					{
						std::string scriptName = script.first.as<std::string>();
						auto scriptInstance = MakeRef<ScriptInstance>(ScriptingSystem::GetScriptClass(scriptName));
						scriptInstance->Init(loadedEntity.GetUUID()); // Initialise the script instance (set the entity reference
						sc.AddScript(scriptName, scriptInstance);

						const YAML::Node& fields = script.second;
						if (fields) {
							for (const auto& field : fields) {
								// Each field will have a name and a corresponding node
								std::string fieldName = field.first.as<std::string>();
								const YAML::Node& fieldData = field.second;
								fieldData["Type"].as<std::string>();

								if (fieldData["Type"].as<std::string>() == "Bool")
								{
									bool data = fieldData["Data"].as<bool>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Float")
								{
									float data = fieldData["Data"].as<float>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Int")
								{
									int data = fieldData["Data"].as<int>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "String")
								{
									std::string data = fieldData["Data"].as<std::string>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Vector2")
								{
									glm::vec2 data = fieldData["Data"].as<glm::vec2>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Vector3")
								{
									glm::vec3 data = fieldData["Data"].as<glm::vec3>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Vector4")
								{
									glm::vec4 data = fieldData["Data"].as<glm::vec4>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "UChar")
								{
									unsigned char data = static_cast<unsigned char>(fieldData["Data"].as<unsigned>());
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Char")
								{
									char data = fieldData["Data"].as<char>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "UShort")
								{
									unsigned short data = fieldData["Data"].as<unsigned short>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Short")
								{
									short data = fieldData["Data"].as<short>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "UInt")
								{
									unsigned data = fieldData["Data"].as<unsigned>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Long")
								{
									long long data = fieldData["Data"].as<long long>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "ULong")
								{
									unsigned long long data = fieldData["Data"].as<unsigned long long>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

								if (fieldData["Type"].as<std::string>() == "Double")
								{
									double data = fieldData["Data"].as<double>();
									scriptInstance->SetFieldValue(fieldName, &data);
									continue;
								}

							}
						}
					}
				}
			}
		}

		return true;
	}

	void Serialiser::SerialisePrefab(const std::string& filepath, Entity entity)
	{
		YAML::Emitter out;
	
		SerializeEntity(out, entity);

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
	}

	bool Serialiser::DeserialiseEditorStyle()
	{
		std::string filepath = "settings/style.setting";
		std::ifstream inStream(filepath);
		std::stringstream ss;
		ss << inStream.rdbuf();
		inStream.close();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Styles"])
		{
			BOREALIS_CORE_ERROR("Style not found in file: {}", filepath);
			return false;
		}

		auto lightMode = data["LightMode"];

		sImGuiLightColours[ImGuiCol_Text] = lightMode["Text"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextDisabled] = lightMode["TextDisabled"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_WindowBg] = lightMode["WindowBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ChildBg] = lightMode["ChildBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PopupBg] = lightMode["PopupBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Border] = lightMode["Border"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_BorderShadow] = lightMode["BorderShadow"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBg] = lightMode["FrameBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBgHovered] = lightMode["FrameBgHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBgActive] = lightMode["FrameBgActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBg] = lightMode["TitleBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBgActive] = lightMode["TitleBgActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBgCollapsed] = lightMode["TitleBgCollapsed"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_MenuBarBg] = lightMode["MenuBarBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarBg] = lightMode["ScrollbarBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrab] = lightMode["ScrollbarGrab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrabHovered] = lightMode["ScrollbarGrabHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrabActive] = lightMode["ScrollbarGrabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_CheckMark] = lightMode["CheckMark"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SliderGrab] = lightMode["SliderGrab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SliderGrabActive] = lightMode["SliderGrabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Button] = lightMode["Button"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ButtonHovered] = lightMode["ButtonHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ButtonActive] = lightMode["ButtonActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Header] = lightMode["Header"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_HeaderHovered] = lightMode["HeaderHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_HeaderActive] = lightMode["HeaderActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Separator] = lightMode["Separator"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SeparatorHovered] = lightMode["SeparatorHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SeparatorActive] = lightMode["SeparatorActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGrip] = lightMode["ResizeGrip"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGripHovered] = lightMode["ResizeGripHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGripActive] = lightMode["ResizeGripActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Tab] = lightMode["Tab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabHovered] = lightMode["TabHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabActive] = lightMode["TabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabUnfocused] = lightMode["TabUnfocused"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabUnfocusedActive] = lightMode["TabUnfocusedActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabSelected] = lightMode["TabSelected"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabSelectedOverline] = lightMode["TabSelectedOverline"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmed] = lightMode["TabDimmed"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmedSelected] = lightMode["TabDimmedSelected"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmedSelectedOverline] = lightMode["TabDimmedSelectedOverline"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotLines] = lightMode["PlotLines"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotLinesHovered] = lightMode["PlotLinesHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotHistogram] = lightMode["PlotHistogram"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotHistogramHovered] = lightMode["PlotHistogramHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextSelectedBg] = lightMode["TextSelectedBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DragDropTarget] = lightMode["DragDropTarget"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavHighlight] = lightMode["NavHighlight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavWindowingHighlight] = lightMode["NavWindowingHighlight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavWindowingDimBg] = lightMode["NavWindowingDimBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ModalWindowDimBg] = lightMode["ModalWindowDimBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DockingEmptyBg] = lightMode["DockingEmptyBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DockingPreview] = lightMode["DockingPreview"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableHeaderBg] = lightMode["TableHeaderBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableBorderStrong] = lightMode["TableBorderStrong"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableBorderLight] = lightMode["TableBorderLight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableRowBg] = lightMode["TableRowBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableRowBgAlt] = lightMode["TableRowBackgroundAlt"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextLink] = lightMode["TextLink"].as<ImVec4>();

		auto darkMode = data["DarkMode"];
		sImGuiDarkColours[ImGuiCol_Text] = darkMode["Text"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextDisabled] = darkMode["TextDisabled"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_WindowBg] = darkMode["WindowBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ChildBg] = darkMode["ChildBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PopupBg] = darkMode["PopupBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Border] = darkMode["Border"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_BorderShadow] = darkMode["BorderShadow"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBg] = darkMode["FrameBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBgHovered] = darkMode["FrameBgHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBgActive] = darkMode["FrameBgActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBg] = darkMode["TitleBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBgActive] = darkMode["TitleBgActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBgCollapsed] = darkMode["TitleBgCollapsed"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_MenuBarBg] = darkMode["MenuBarBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarBg] = darkMode["ScrollbarBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrab] = darkMode["ScrollbarGrab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrabHovered] = darkMode["ScrollbarGrabHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrabActive] = darkMode["ScrollbarGrabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_CheckMark] = darkMode["CheckMark"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SliderGrab] = darkMode["SliderGrab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SliderGrabActive] = darkMode["SliderGrabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Button] = darkMode["Button"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ButtonHovered] = darkMode["ButtonHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ButtonActive] = darkMode["ButtonActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Header] = darkMode["Header"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_HeaderHovered] = darkMode["HeaderHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_HeaderActive] = darkMode["HeaderActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Separator] = darkMode["Separator"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SeparatorHovered] = darkMode["SeparatorHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SeparatorActive] = darkMode["SeparatorActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGrip] = darkMode["ResizeGrip"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGripHovered] = darkMode["ResizeGripHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGripActive] = darkMode["ResizeGripActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Tab] = darkMode["Tab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabHovered] = darkMode["TabHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabActive] = darkMode["TabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabUnfocused] = darkMode["TabUnfocused"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabUnfocusedActive] = darkMode["TabUnfocusedActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabSelected] = darkMode["TabSelected"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabSelectedOverline] = darkMode["TabSelectedOverline"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmed] = darkMode["TabDimmed"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmedSelected] = darkMode["TabDimmedSelected"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmedSelectedOverline] = darkMode["TabDimmedSelectedOverline"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotLines] = darkMode["PlotLines"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotLinesHovered] = darkMode["PlotLinesHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotHistogram] = darkMode["PlotHistogram"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotHistogramHovered] = darkMode["PlotHistogramHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextSelectedBg] = darkMode["TextSelectedBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DragDropTarget] = darkMode["DragDropTarget"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavHighlight] = darkMode["NavHighlight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavWindowingHighlight] = darkMode["NavWindowingHighlight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavWindowingDimBg] = darkMode["NavWindowingDimBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ModalWindowDimBg] = darkMode["ModalWindowDimBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DockingEmptyBg] = darkMode["DockingEmptyBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DockingPreview] = darkMode["DockingPreview"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableHeaderBg] = darkMode["TableHeaderBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableBorderStrong] = darkMode["TableBorderStrong"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableBorderLight] = darkMode["TableBorderLight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableRowBg] = darkMode["TableRowBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableRowBgAlt] = darkMode["TableRowBackgroundAlt"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextLink] = darkMode["TextLink"].as<ImVec4>();

		return true;
	}


	bool Serialiser::SerialiseEditorStyle()
	{
		std::string filepath = "settings/style.setting";
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Styles"  << YAML::Value << "StyleType: Light/Dark"
			<< YAML::Key << "LightMode" << YAML::Value << YAML::BeginMap;


		ImGuiStyle& style = ImGui::GetStyle();
		out << YAML::Key << "Text" << YAML::Value << style.Colors[ImGuiCol_Text];
		out << YAML::Key << "TextDisabled" << YAML::Value << style.Colors[ImGuiCol_TextDisabled];
		out << YAML::Key << "WindowBg" << YAML::Value << style.Colors[ImGuiCol_WindowBg];
		out << YAML::Key << "ChildBg" << YAML::Value << style.Colors[ImGuiCol_ChildBg];
		out << YAML::Key << "PopupBg" << YAML::Value << style.Colors[ImGuiCol_PopupBg];
		out << YAML::Key << "Border" << YAML::Value << style.Colors[ImGuiCol_Border];
		out << YAML::Key << "BorderShadow" << YAML::Value << style.Colors[ImGuiCol_BorderShadow];
		out << YAML::Key << "FrameBg" << YAML::Value << style.Colors[ImGuiCol_FrameBg];
		out << YAML::Key << "FrameBgHovered" << YAML::Value << style.Colors[ImGuiCol_FrameBgHovered];
		out << YAML::Key << "FrameBgActive" << YAML::Value << style.Colors[ImGuiCol_FrameBgActive];
		out << YAML::Key << "TitleBg" << YAML::Value << style.Colors[ImGuiCol_TitleBg];	
		out << YAML::Key << "TitleBgActive" << YAML::Value << style.Colors[ImGuiCol_TitleBgActive];
		out << YAML::Key << "TitleBgCollapsed" << YAML::Value << style.Colors[ImGuiCol_TitleBgCollapsed];
		out << YAML::Key << "MenuBarBg" << YAML::Value << style.Colors[ImGuiCol_MenuBarBg];
		out << YAML::Key << "ScrollbarBg" << YAML::Value << style.Colors[ImGuiCol_ScrollbarBg];
		out << YAML::Key << "ScrollbarGrab" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrab];
		out << YAML::Key << "ScrollbarGrabHovered" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabHovered];
		out << YAML::Key << "ScrollbarGrabActive" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabActive];
		out << YAML::Key << "CheckMark" << YAML::Value << style.Colors[ImGuiCol_CheckMark];
		out << YAML::Key << "SliderGrab" << YAML::Value << style.Colors[ImGuiCol_SliderGrab];
		out << YAML::Key << "SliderGrabActive" << YAML::Value << style.Colors[ImGuiCol_SliderGrabActive];
		out << YAML::Key << "Button" << YAML::Value << style.Colors[ImGuiCol_Button];
		out << YAML::Key << "ButtonHovered" << YAML::Value << style.Colors[ImGuiCol_ButtonHovered];
		out << YAML::Key << "ButtonActive" << YAML::Value << style.Colors[ImGuiCol_ButtonActive];
		out << YAML::Key << "Header" << YAML::Value << style.Colors[ImGuiCol_Header];
		out << YAML::Key << "HeaderHovered" << YAML::Value << style.Colors[ImGuiCol_HeaderHovered];
		out << YAML::Key << "HeaderActive" << YAML::Value << style.Colors[ImGuiCol_HeaderActive];
		out << YAML::Key << "Separator" << YAML::Value << style.Colors[ImGuiCol_Separator];
		out << YAML::Key << "SeparatorHovered" << YAML::Value << style.Colors[ImGuiCol_SeparatorHovered];
		out << YAML::Key << "SeparatorActive" << YAML::Value << style.Colors[ImGuiCol_SeparatorActive];
		out << YAML::Key << "ResizeGrip" << YAML::Value << style.Colors[ImGuiCol_ResizeGrip];
		out << YAML::Key << "ResizeGripHovered" << YAML::Value << style.Colors[ImGuiCol_ResizeGripHovered];
		out << YAML::Key << "ResizeGripActive" << YAML::Value << style.Colors[ImGuiCol_ResizeGripActive];
		out << YAML::Key << "Tab" << YAML::Value << style.Colors[ImGuiCol_Tab];
		out << YAML::Key << "TabHovered" << YAML::Value << style.Colors[ImGuiCol_TabHovered];
		out << YAML::Key << "TabActive" << YAML::Value << style.Colors[ImGuiCol_TabActive];
		out << YAML::Key << "TabUnfocused" << YAML::Value << style.Colors[ImGuiCol_TabUnfocused];
		out << YAML::Key << "TabUnfocusedActive" << YAML::Value << style.Colors[ImGuiCol_TabUnfocusedActive];
		out << YAML::Key << "TabSelected" << YAML::Value << style.Colors[ImGuiCol_TabSelected];
		out << YAML::Key << "TabSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabSelectedOverline];
		out << YAML::Key << "TabDimmed" << YAML::Value << style.Colors[ImGuiCol_TabDimmed];
		out << YAML::Key << "TabDimmedSelected" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelected];
		out << YAML::Key << "TabDimmedSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelectedOverline];
		out << YAML::Key << "PlotLines" << YAML::Value << style.Colors[ImGuiCol_PlotLines];
		out << YAML::Key << "PlotLinesHovered" << YAML::Value << style.Colors[ImGuiCol_PlotLinesHovered];
		out << YAML::Key << "PlotHistogram" << YAML::Value << style.Colors[ImGuiCol_PlotHistogram];
		out << YAML::Key << "PlotHistogramHovered" << YAML::Value << style.Colors[ImGuiCol_PlotHistogramHovered];
		out << YAML::Key << "TextSelectedBg" << YAML::Value << style.Colors[ImGuiCol_TextSelectedBg];
		out << YAML::Key << "DragDropTarget" << YAML::Value << style.Colors[ImGuiCol_DragDropTarget];
		out << YAML::Key << "NavHighlight" << YAML::Value << style.Colors[ImGuiCol_NavHighlight];
		out << YAML::Key << "NavWindowingHighlight" << YAML::Value << style.Colors[ImGuiCol_NavWindowingHighlight];
		out << YAML::Key << "NavWindowingDimBg" << YAML::Value << style.Colors[ImGuiCol_NavWindowingDimBg];
		out << YAML::Key << "ModalWindowDimBg" << YAML::Value << style.Colors[ImGuiCol_ModalWindowDimBg];
		out << YAML::Key << "DockingEmptyBackground" << YAML::Value << style.Colors[ImGuiCol_DockingEmptyBg];
		out << YAML::Key << "DockingPreview" << YAML::Value << style.Colors[ImGuiCol_DockingPreview];
		out << YAML::Key << "TableHeaderBackground" << YAML::Value << style.Colors[ImGuiCol_TableHeaderBg];
		out << YAML::Key << "TableBorderStrong" << YAML::Value << style.Colors[ImGuiCol_TableBorderStrong];
		out << YAML::Key << "TableBorderLight" << YAML::Value << style.Colors[ImGuiCol_TableBorderLight];
		out << YAML::Key << "TableRowBackground" << YAML::Value << style.Colors[ImGuiCol_TableRowBg];
		out << YAML::Key << "TableRowBackgroundAlt" << YAML::Value << style.Colors[ImGuiCol_TableRowBgAlt];
		out << YAML::Key << "TextLink" << YAML::Value << style.Colors[ImGuiCol_TextLink];

		out << YAML::EndMap;

		out << YAML::Key << "DarkMode" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Text" << YAML::Value << style.Colors[ImGuiCol_Text];
		out << YAML::Key << "TextDisabled" << YAML::Value << style.Colors[ImGuiCol_TextDisabled];
		out << YAML::Key << "WindowBg" << YAML::Value << style.Colors[ImGuiCol_WindowBg];
		out << YAML::Key << "ChildBg" << YAML::Value << style.Colors[ImGuiCol_ChildBg];
		out << YAML::Key << "PopupBg" << YAML::Value << style.Colors[ImGuiCol_PopupBg];
		out << YAML::Key << "Border" << YAML::Value << style.Colors[ImGuiCol_Border];
		out << YAML::Key << "BorderShadow" << YAML::Value << style.Colors[ImGuiCol_BorderShadow];
		out << YAML::Key << "FrameBg" << YAML::Value << style.Colors[ImGuiCol_FrameBg];
		out << YAML::Key << "FrameBgHovered" << YAML::Value << style.Colors[ImGuiCol_FrameBgHovered];
		out << YAML::Key << "FrameBgActive" << YAML::Value << style.Colors[ImGuiCol_FrameBgActive];
		out << YAML::Key << "TitleBg" << YAML::Value << style.Colors[ImGuiCol_TitleBg];
		out << YAML::Key << "TitleBgActive" << YAML::Value << style.Colors[ImGuiCol_TitleBgActive];
		out << YAML::Key << "TitleBgCollapsed" << YAML::Value << style.Colors[ImGuiCol_TitleBgCollapsed];
		out << YAML::Key << "MenuBarBg" << YAML::Value << style.Colors[ImGuiCol_MenuBarBg];
		out << YAML::Key << "ScrollbarBg" << YAML::Value << style.Colors[ImGuiCol_ScrollbarBg];
		out << YAML::Key << "ScrollbarGrab" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrab];
		out << YAML::Key << "ScrollbarGrabHovered" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabHovered];
		out << YAML::Key << "ScrollbarGrabActive" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabActive];
		out << YAML::Key << "CheckMark" << YAML::Value << style.Colors[ImGuiCol_CheckMark];
		out << YAML::Key << "SliderGrab" << YAML::Value << style.Colors[ImGuiCol_SliderGrab];
		out << YAML::Key << "SliderGrabActive" << YAML::Value << style.Colors[ImGuiCol_SliderGrabActive];
		out << YAML::Key << "Button" << YAML::Value << style.Colors[ImGuiCol_Button];
		out << YAML::Key << "ButtonHovered" << YAML::Value << style.Colors[ImGuiCol_ButtonHovered];
		out << YAML::Key << "ButtonActive" << YAML::Value << style.Colors[ImGuiCol_ButtonActive];
		out << YAML::Key << "Header" << YAML::Value << style.Colors[ImGuiCol_Header];
		out << YAML::Key << "HeaderHovered" << YAML::Value << style.Colors[ImGuiCol_HeaderHovered];
		out << YAML::Key << "HeaderActive" << YAML::Value << style.Colors[ImGuiCol_HeaderActive];
		out << YAML::Key << "Separator" << YAML::Value << style.Colors[ImGuiCol_Separator];
		out << YAML::Key << "SeparatorHovered" << YAML::Value << style.Colors[ImGuiCol_SeparatorHovered];
		out << YAML::Key << "SeparatorActive" << YAML::Value << style.Colors[ImGuiCol_SeparatorActive];
		out << YAML::Key << "ResizeGrip" << YAML::Value << style.Colors[ImGuiCol_ResizeGrip];
		out << YAML::Key << "ResizeGripHovered" << YAML::Value << style.Colors[ImGuiCol_ResizeGripHovered];
		out << YAML::Key << "ResizeGripActive" << YAML::Value << style.Colors[ImGuiCol_ResizeGripActive];
		out << YAML::Key << "Tab" << YAML::Value << style.Colors[ImGuiCol_Tab];
		out << YAML::Key << "TabHovered" << YAML::Value << style.Colors[ImGuiCol_TabHovered];
		out << YAML::Key << "TabActive" << YAML::Value << style.Colors[ImGuiCol_TabActive];
		out << YAML::Key << "TabUnfocused" << YAML::Value << style.Colors[ImGuiCol_TabUnfocused];
		out << YAML::Key << "TabUnfocusedActive" << YAML::Value << style.Colors[ImGuiCol_TabUnfocusedActive];
		out << YAML::Key << "TabSelected" << YAML::Value << style.Colors[ImGuiCol_TabSelected];
		out << YAML::Key << "TabSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabSelectedOverline];
		out << YAML::Key << "TabDimmed" << YAML::Value << style.Colors[ImGuiCol_TabDimmed];
		out << YAML::Key << "TabDimmedSelected" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelected];
		out << YAML::Key << "TabDimmedSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelectedOverline];
		out << YAML::Key << "PlotLines" << YAML::Value << style.Colors[ImGuiCol_PlotLines];
		out << YAML::Key << "PlotLinesHovered" << YAML::Value << style.Colors[ImGuiCol_PlotLinesHovered];
		out << YAML::Key << "PlotHistogram" << YAML::Value << style.Colors[ImGuiCol_PlotHistogram];
		out << YAML::Key << "PlotHistogramHovered" << YAML::Value << style.Colors[ImGuiCol_PlotHistogramHovered];
		out << YAML::Key << "TextSelectedBg" << YAML::Value << style.Colors[ImGuiCol_TextSelectedBg];
		out << YAML::Key << "DragDropTarget" << YAML::Value << style.Colors[ImGuiCol_DragDropTarget];
		out << YAML::Key << "NavHighlight" << YAML::Value << style.Colors[ImGuiCol_NavHighlight];
		out << YAML::Key << "NavWindowingHighlight" << YAML::Value << style.Colors[ImGuiCol_NavWindowingHighlight];
		out << YAML::Key << "NavWindowingDimBg" << YAML::Value << style.Colors[ImGuiCol_NavWindowingDimBg];
		out << YAML::Key << "ModalWindowDimBg" << YAML::Value << style.Colors[ImGuiCol_ModalWindowDimBg];
		out << YAML::Key << "DockingEmptyBackground" << YAML::Value << style.Colors[ImGuiCol_DockingEmptyBg];
		out << YAML::Key << "DockingPreview" << YAML::Value << style.Colors[ImGuiCol_DockingPreview];
		out << YAML::Key << "TableHeaderBackground" << YAML::Value << style.Colors[ImGuiCol_TableHeaderBg];
		out << YAML::Key << "TableBorderStrong" << YAML::Value << style.Colors[ImGuiCol_TableBorderStrong];
		out << YAML::Key << "TableBorderLight" << YAML::Value << style.Colors[ImGuiCol_TableBorderLight];
		out << YAML::Key << "TableRowBackground" << YAML::Value << style.Colors[ImGuiCol_TableRowBg];
		out << YAML::Key << "TableRowBackgroundAlt" << YAML::Value << style.Colors[ImGuiCol_TableRowBgAlt];
		out << YAML::Key << "TextLink" << YAML::Value << style.Colors[ImGuiCol_TextLink];

		out << YAML::EndMap
			<< YAML::EndMap;

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
		return true;
	}
}