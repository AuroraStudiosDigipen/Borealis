#pragma once
#include <unordered_map>
#include <unordered_set>
#include <Scene/Entity.hpp>
#include <Core/Core.hpp>
#include <entt.hpp>
#include <PrefabManager.hpp>

//PrefabManager creates the ECS to store all prefabs as ECS objects
namespace Borealis
{
	class Prefab;
	class PrefabManager
	{
	public:
		static entt::registry& GetRegistry() { return mPrefabScene.GetRegistry(); }
		static entt::entity CreateEntity();

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
