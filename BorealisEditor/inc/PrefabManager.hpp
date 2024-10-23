#pragma once
#include <unordered_map>
#include <unordered_set>
#include <Scene/Entity.hpp>
#include <Core/Core.hpp>
#include <entt.hpp>
#include <PrefabManager.hpp>
#include <yaml-cpp/yaml.h>

//PrefabManager creates the ECS to store all prefabs as ECS objects
namespace Borealis
{
	class Prefab;
	class PrefabManager
	{
	public:
		static entt::registry& GetRegistry() { return mPrefabScene.GetRegistry(); }
		static entt::entity CreateEntity();

		static entt::entity InstantiatePrefabInstance(Ref<Prefab> prefab, const Scene& scene);

		static void ShowAllPrefabs()
		{
			std::cout << "Listing all Prefabs in the Prefab Manager:" << std::endl;
			for (const auto& [uuid, prefab] : mPrefabs)
			{
				std::cout << "Prefab UUID: " << uuid << std::endl;

				//if (prefab->HasComponent<TransformComponent>())
				//{
				//	std::cout << "  - HAS TransformComponent" << std::endl;
				//}
				//if (prefab->HasComponent<SpriteRendererComponent>())
				//{
				//	std::cout << "  - HAS SpriteRendererComponent" << std::endl;
				//}
			}
		}

		
		//Taking of UUID for creating prefabs base from registry
		static UUID ExtractUUIDFromFile(const std::string& filepath)
		{
			// Open the file
			std::ifstream inStream(filepath);
			if (!inStream.is_open())
			{
				std::cerr << "Failed to open file: " << filepath << std::endl;
				return UUID(); // Return an empty or invalid UUID
			}

			// Read the file content into a stringstream
			std::stringstream ss;
			ss << inStream.rdbuf();
			inStream.close(); // Close the file after reading

			// Load the YAML content
			YAML::Node data = YAML::Load(ss.str());

			// Extract the EntityID (UUID)
			if (data["EntityID"])
			{
				uint64_t uuid = data["EntityID"].as<uint64_t>();
				std::cout << "Extracted EntityID (UUID): " << uuid << std::endl;

				// Return the extracted UUID
				return UUID(uuid);
			}
			else
			{
				std::cerr << "EntityID not found in file: " << filepath << std::endl;
				return UUID(); // Return an empty or invalid UUID if not found
			}
		}

		template <typename T, typename ...Args>
		static T& AddComponent(entt::entity mPrefabID, Args&&... args)
		{
			T& Component = mPrefabScene.GetRegistry().emplace<T>(mPrefabID, std::forward<Args>(args)...);
			return Component;
		}

		template<typename T>
		static bool HasComponent(entt::entity mPrefabID)
		{
			return mPrefabScene.GetRegistry().storage<T>().contains(mPrefabID);
		}

		template<typename T>
		static T& GetComponent(entt::entity mPrefabID)
		{
			return mPrefabScene.GetRegistry().get<T>(mPrefabID);
		}

		template<typename T>
		static void RemoveComponent(entt::entity mPrefabID)
		{
			mPrefabScene.GetRegistry().remove<T>(mPrefabID);
		}

		template <typename T, typename...Args>
		static T& AddOrReplaceComponent(entt::entity mPrefabID, Args&& ... args)
		{
			T& Component = mPrefabScene.GetRegistry().emplace_or_replace<T>(mPrefabID, std::forward<Args>(args)...);
			return Component;
		}




		static void Register(Ref<Prefab>prefab);

		static Scene* GetScenePtr() { return &mPrefabScene; }

		static Ref<Prefab> GetPrefab(UUID mPrefabID) { return mPrefabs[mPrefabID]; }

		static void DeserialisePrefab(std::string path);

	private:
		static std::unordered_map<UUID, Ref<Prefab>> mPrefabs;
		static Scene mPrefabScene;
	};
}
