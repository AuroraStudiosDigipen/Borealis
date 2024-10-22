#include <Scene/Entity.hpp>
#include <Core/Core.hpp>
#include <Scene/Serialiser.hpp>
#include <PrefabManager.hpp>
#include <Prefab.hpp>
#include <Assets/AssetManager.hpp>
#include <imgui.h>

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
	std::unordered_map<UUID, Ref<Prefab>> PrefabManager::mPrefabs;
	Scene PrefabManager::mPrefabScene;
	entt::entity PrefabManager::CreateEntity()
	{
		return mPrefabScene.GetRegistry().create();
	}

	void PrefabManager::Register(Ref<Prefab> prefab)
	{
		mPrefabs[prefab->GetUUID()] = prefab;
	}

	void PrefabManager::DeserialisePrefab(std::string path)
	{
		std::ifstream inStream(path);
		std::stringstream ss;
		ss << inStream.rdbuf();
		inStream.close();

		YAML::Node data = YAML::Load(ss.str());
		uint64_t uuid = data["EntityID"].as<uint64_t>(); // UUID

		auto tagComponent = data["TagComponent"];
		std::string name;
		if (tagComponent)
		{
			name = tagComponent["Tag"].as<std::string>();
		}

		Prefab loadedEntity(uuid);
		loadedEntity.GetComponent<TagComponent>().Tag = name;

		auto transformComponent = data["TransformComponent"];
		if (transformComponent)
		{
			auto& tc = loadedEntity.GetComponent<TransformComponent>();
			tc.Translate = data["TransformComponent"]["Translate"].as<glm::vec3>();
			tc.Rotation = data["TransformComponent"]["Rotation"].as<glm::vec3>();
			tc.Scale = data["TransformComponent"]["Scale"].as<glm::vec3>();
		}

		auto spriteRendererComponent = data["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& src = loadedEntity.AddComponent<SpriteRendererComponent>();
			src.Colour = data["SpriteRendererComponent"]["Colour"].as<glm::vec4>();
			if (data["SpriteRendererComponent"]["Texture"].IsDefined())
			{
				src.Texture = AssetManager::GetAsset<Texture2D>(data["SpriteRendererComponent"]["Texture"].as<uint64_t>());
			}
		}

		auto circleRendererComponent = data["CircleRendererComponent"];
		if (circleRendererComponent)
		{
			auto& src = loadedEntity.AddComponent<CircleRendererComponent>();
			src.Colour = data["CircleRendererComponent"]["Colour"].as<glm::vec4>();
			src.thickness = data["CircleRendererComponent"]["Thickness"].as<float>();
			src.fade = data["CircleRendererComponent"]["Fade"].as<float>();
		}

		auto cameraComponent = data["CameraComponent"];
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

		auto meshFilterComponent = data["MeshFilterComponent"];
		if (meshFilterComponent)
		{
			auto& mfc = loadedEntity.AddComponent<MeshFilterComponent>();
			uint64_t uuid = data["MeshFilterComponent"]["Mesh"].as<uint64_t>(); // UUID
			mfc.Model = AssetManager::GetAsset<Model>(uuid); // TODO: Load Mesh via UUID
		}

		auto meshRendererComponent = data["MeshRendererComponent"];
		if (meshRendererComponent)
		{
			auto& mrc = loadedEntity.AddComponent<MeshRendererComponent>();
			uint64_t uuid = data["MeshRendererComponent"]["Material"].as<uint64_t>();
			mrc.Material = AssetManager::GetAsset<Material>(uuid);
			mrc.castShadow = meshRendererComponent["CastShadow"].as<bool>();
		}

		auto boxColliderComponent = data["BoxColliderComponent"];
		if (boxColliderComponent)
		{
			auto& bcc = loadedEntity.AddComponent<BoxColliderComponent>();
			bcc.isTrigger = boxColliderComponent["isTrigger"].as<bool>();
			bcc.providesContact = boxColliderComponent["providesContact"].as<bool>();
			bcc.Material = nullptr; // TODO: Load Material via UUID
			bcc.Center = boxColliderComponent["Center"].as<glm::vec3>();
			bcc.Size = boxColliderComponent["Size"].as<glm::vec3>();
		}

		auto capsuleColliderComponent = data["CapsuleColliderComponent"];
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

		auto rigidBodyComponent = data["RigidBodyComponent"];
		if (rigidBodyComponent)
		{
			auto& rbc = loadedEntity.AddComponent<RigidBodyComponent>();
			//rbc.mass = rigidBodyComponent["mass"].as<float>();
			//rbc.drag = rigidBodyComponent["drag"].as<float>();
			//rbc.angularDrag = rigidBodyComponent["angularDrag"].as<float>();
			//rbc.centerOfMass = rigidBodyComponent["centerOfMass"].as<glm::vec3>();
			//rbc.inertiaTensor = rigidBodyComponent["inertiaTensor"].as<glm::vec3>();
			//rbc.inertiaTensorRotation = rigidBodyComponent["inertiaTensorRotation"].as<glm::vec3>();
			//rbc.AutomaticCenterOfMass = rigidBodyComponent["AutomaticCenterOfMass"].as<bool>();
			//rbc.AutomaticTensor = rigidBodyComponent["AutomaticTensor"].as<bool>();
			//rbc.useGravity = rigidBodyComponent["useGravity"].as<bool>();
			//rbc.isKinematic = rigidBodyComponent["isKinematic"].as<bool>();
		}

		auto lightComponent = data["LightComponent"];
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

		mPrefabs[uuid] = MakeRef<Prefab>(loadedEntity);
	}

}

