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
#include <Scene/Serialiser.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <Core/LoggerSystem.hpp>
#include <ImGui/ImGuiFontLib.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <Assets/AssetManager.hpp>

namespace YAML
{
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) return false;
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

	static void SerializeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID();
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tagComponent = entity.GetComponent<TagComponent>();
			out << YAML::Key << "Tag" << YAML::Value << tagComponent.Tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "CameraType" << YAML::Value << (int)camera.GetCameraType();
			out << YAML::Key << "OrthoSize" << YAML::Value << camera.GetOrthoSize();
			out << YAML::Key << "OrthoNear" << YAML::Value << camera.GetOrthoNear();
			out << YAML::Key << "OrthoFar" << YAML::Value << camera.GetOrthoFar();
			out << YAML::Key << "PerspFOV" << YAML::Value << camera.GetPerspFOV();
			out << YAML::Key << "PerspNear" << YAML::Value << camera.GetPerspNear();
			out << YAML::Key << "PerspFar" << YAML::Value << camera.GetPerspFar();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translate" << YAML::Value << transformComponent.Translate;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Colour" << YAML::Value << spriteRendererComponent.Colour;


			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Colour" << YAML::Value << circleRendererComponent.Colour;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshFilterComponent>())
		{
			out << YAML::Key << "MeshFilterComponent";
			out << YAML::BeginMap;

			auto& meshFilterComponent = entity.GetComponent<MeshFilterComponent>();
			out << YAML::Key << "Mesh" << YAML::Value << meshFilterComponent.Model->mAssetHandle; //UUID of Mesh
			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap;

			auto& meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
			out << YAML::Key << "Material" << YAML::Value << meshRendererComponent.Material->mAssetHandle;
			out << YAML::Key << "CastShadow" << YAML::Value << meshRendererComponent.castShadow;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxColliderComponent>())
		{
			out << YAML::Key << "BoxColliderComponent";
			out << YAML::BeginMap;

			auto& boxColliderComponent = entity.GetComponent<BoxColliderComponent>();
			out << YAML::Key << "isTrigger" << YAML::Value << boxColliderComponent.isTrigger; 
			out << YAML::Key << "providesContact" << YAML::Value << boxColliderComponent.providesContact;
			out << YAML::Key << "PhysicMaterial" << YAML::Value << 34256545; // UUID of material
			out << YAML::Key << "Center" << YAML::Value << boxColliderComponent.Center;
			out << YAML::Key << "Size" << YAML::Value << boxColliderComponent.Size;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			out << YAML::Key << "CapsuleColliderComponent";
			out << YAML::BeginMap;

			auto& capsuleColliderComponent = entity.GetComponent<CapsuleColliderComponent>();
			out << YAML::Key << "isTrigger" << YAML::Value << capsuleColliderComponent.isTrigger;
			out << YAML::Key << "providesContact" << YAML::Value << capsuleColliderComponent.providesContact;
			out << YAML::Key << "PhysicMaterial" << YAML::Value << 34256545; // UUID of material
			out << YAML::Key << "Radius" << YAML::Value << capsuleColliderComponent.radius;
			out << YAML::Key << "Height" << YAML::Value << capsuleColliderComponent.height;
			out << YAML::Key << "Direction" << YAML::Value << (int)capsuleColliderComponent.direction;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<RigidBodyComponent>())
		{
			out << YAML::Key << "RigidBodyComponent";
			out << YAML::BeginMap;

			auto& rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();
			out << YAML::Key << "mass" << YAML::Value << rigidBodyComponent.mass;
			out << YAML::Key << "drag" << YAML::Value << rigidBodyComponent.drag;
			out << YAML::Key << "angularDrag" << YAML::Value << rigidBodyComponent.angularDrag;
			out << YAML::Key << "centerOfMass" << YAML::Value << rigidBodyComponent.centerOfMass;
			out << YAML::Key << "inertiaTensor" << YAML::Value << rigidBodyComponent.inertiaTensor;
			out << YAML::Key << "inertiaTensorRotation" << YAML::Value << rigidBodyComponent.inertiaTensorRotation;
			out << YAML::Key << "AutomaticCenterOfMass" << YAML::Value << rigidBodyComponent.AutomaticCenterOfMass;
			out << YAML::Key << "AutomaticTensor" << YAML::Value << rigidBodyComponent.AutomaticTensor;
			out << YAML::Key << "useGravity" << YAML::Value << rigidBodyComponent.useGravity;
			out << YAML::Key << "isKinematic" << YAML::Value << rigidBodyComponent.isKinematic;

			out << YAML::EndMap;
		}


		if (entity.HasComponent<LightComponent>())
		{
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;

			auto& lightComponent = entity.GetComponent<LightComponent>();
			out << YAML::Key << "Ambient" << YAML::Value << lightComponent.ambient;
			out << YAML::Key << "Diffuse" << YAML::Value << lightComponent.diffuse;
			out << YAML::Key << "Direction" << YAML::Value << lightComponent.direction;
			out << YAML::Key << "Specular" << YAML::Value << lightComponent.specular;
			out << YAML::Key << "InnerSpotX" << YAML::Value << lightComponent.InnerOuterSpot.x;
			out << YAML::Key << "InnerSpotY" << YAML::Value << lightComponent.InnerOuterSpot.y;
			out << YAML::Key << "Linear" << YAML::Value << lightComponent.linear;
			out << YAML::Key << "Quadratic" << YAML::Value << lightComponent.quadratic;
			out << YAML::Key << "Specular" << YAML::Value << lightComponent.specular;
			out << YAML::Key << "Type" << YAML::Value << (int)lightComponent.type;

			/*out << YAML::Key << "Colour" << YAML::Value << lightComponent.Colour;
			out << YAML::Key << "InnerSpot" << YAML::Value << lightComponent.InnerOuterSpot.x;
			out << YAML::Key << "OuterSpot" << YAML::Value << lightComponent.InnerOuterSpot.y;
			out << YAML::Key << "Temperature" << YAML::Value << lightComponent.Temperature;
			out << YAML::Key << "Intensity" << YAML::Value << lightComponent.Intensity;
			out << YAML::Key << "IndirectMultiplier" << YAML::Value << lightComponent.IndirectMultiplier;
			out << YAML::Key << "Range" << YAML::Value << lightComponent.Range;
			out << YAML::Key << "Type" << YAML::Value << (int)lightComponent.type;
			out << YAML::Key << "ShadowType" << YAML::Value << (int)lightComponent.shadowType;
			out << YAML::Key << "LightAppearance" << YAML::Value << (int)lightComponent.lightAppearance;*/

			out << YAML::EndMap;
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
					rbc.mass = rigidBodyComponent["mass"].as<float>();
					rbc.drag = rigidBodyComponent["drag"].as<float>();
					rbc.angularDrag = rigidBodyComponent["angularDrag"].as<float>();
					rbc.centerOfMass = rigidBodyComponent["centerOfMass"].as<glm::vec3>();
					rbc.inertiaTensor = rigidBodyComponent["inertiaTensor"].as<glm::vec3>();
					rbc.inertiaTensorRotation = rigidBodyComponent["inertiaTensorRotation"].as<glm::vec3>();
					rbc.AutomaticCenterOfMass = rigidBodyComponent["AutomaticCenterOfMass"].as<bool>();
					rbc.AutomaticTensor = rigidBodyComponent["AutomaticTensor"].as<bool>();
					rbc.useGravity = rigidBodyComponent["useGravity"].as<bool>();
					rbc.isKinematic = rigidBodyComponent["isKinematic"].as<bool>();
				}

				auto lightComponent = entity["LightComponent"];
				if (lightComponent)
				{
					auto& lc = loadedEntity.AddComponent<LightComponent>();
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
			}
		}

		return true;
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