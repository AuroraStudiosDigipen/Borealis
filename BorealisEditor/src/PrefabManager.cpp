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
	Ref<Scene> PrefabManager::mPrefabScene{ MakeRef<Scene>() };
	entt::entity PrefabManager::CreateEntity()
	{
		return mPrefabScene->GetRegistry().create();
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
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				UUID uuid = entity["EntityID"].as<uint64_t>(); // UUID
				Serialiser serialiser(mPrefabScene);
				entt::entity loadedEntity = serialiser.DeserialiseEntity(entity, mPrefabScene->GetRegistry(), uuid);
				Entity borealisEntity(loadedEntity, mPrefabScene.get());
				mPrefabs[uuid] = MakeRef<Prefab>(borealisEntity);
				mPrefabScene->DestroyEntity(borealisEntity);
				mPrefabs[uuid]->GetComponent<IDComponent>().ID = uuid;
			}
		}

	}

}

