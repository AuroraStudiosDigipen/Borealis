/******************************************************************************/
/*!
\file		Prefab.cpp
\author 	Yeo Jun Jie
\par    	email: junjie.yeo@digipen.edu
\date   	September 12, 2024
\brief		Definations of Prefab.h

Contains the definations of the Prefab class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <Scene/Components.hpp>
#include <Scene/ComponentRegistry.hpp>
#include "Prefab.hpp"
#include <PrefabManager.hpp>
#include <iostream>  // For testing/debugging purposes
#include "PrefabComponent.hpp"
#include <Core/LoggerSystem.hpp>

namespace Borealis {


#define AddPrefabComponent(ComponentName) \
	if (entity.HasComponent<ComponentName>()) \
	{ \
		PrefabManager::AddOrReplaceComponent<ComponentName>(mPrefabID, entity.GetComponent<ComponentName>()); \
	} \

#define AddEntityComponent(ComponentName) \
	if (HasComponent<ComponentName>()) \
	{ \
		auto& com = entity->AddComponent<ComponentName>(); \
        com = GetComponent<ComponentName>(); \
	} \


	template<>
	void Scene::OnComponentAdded<PrefabComponent>(Entity entity, PrefabComponent& component)
	{

	}
    Prefab::Prefab(UUID id)
    {
        PrefabManager::CreateEntity();
        mPrefabID = PrefabManager::CreateEntity();
        PrefabManager::AddComponent<IDComponent>(mPrefabID);
        PrefabManager::AddComponent<TagComponent>(mPrefabID);
        PrefabManager::AddComponent<TransformComponent>(mPrefabID);

        PrefabManager::GetRegistry().get<IDComponent>(mPrefabID).ID = id;
    }
    //Creates Prefab base on exisiting entity
	Prefab::Prefab(Entity entity)
	{
        if (!entity.HasComponent<PrefabComponent>())
        {
            auto& prefabComponent = entity.AddComponent<PrefabComponent>();

            mPrefabID = PrefabManager::CreateEntity();
            AddPrefabComponent(TransformComponent);
            AddPrefabComponent(TagComponent);
            AddPrefabComponent(IDComponent);
            AddPrefabComponent(SpriteRendererComponent);

            PrefabManager::GetRegistry().get<IDComponent>(mPrefabID).ID = UUID{}; // Reset the UUID
            prefabComponent.mPrefabID = mPrefabID;
			prefabComponent.mParentID = GetUUID();
            //All created prefab translate wont follow by default
            prefabComponent.mEditedComponentList.insert("Transform Component:Translate");
        }
        mPrefabID = entity.GetComponent<PrefabComponent>().mPrefabID;
		// Add all components to prefab manager ECS

	}

    Entity Prefab::InstantiatePrefabInstance(Ref<Scene> scene) {
        // Create a new entity in the scene
        Entity newEntity = scene->CreateEntity("InstantiatedPrefab");

		//newEntity.AddOrReplaceComponent<PrefabComponent>().mParentID = GetUUID();

        // Copy all components from the prefab to the new entity
        if (HasComponent<SpriteRendererComponent>()) {
            newEntity.AddOrReplaceComponent<SpriteRendererComponent>(GetComponent<SpriteRendererComponent>());
        }

        // Copy all components from the prefab to the new entity
        if (HasComponent<TransformComponent>()) {
            newEntity.AddOrReplaceComponent<TransformComponent>(GetComponent<TransformComponent>());
        }

        mChildren.insert(MakeRef<Entity>(newEntity));

        // Add other components that are part of the prefab as needed

        // Return the newly created entity
        return newEntity;
    }

	void Prefab::AddChild(Ref<Entity> entity)
	{
		//if the entity is valid and the it is not in list
		if (entity && mChildren.find(entity) == mChildren.end())
		{
			mChildren.insert(entity);
		}
	}
	void Prefab::RemoveChild(Ref<Entity> entity)
	{
		//if the entity is valid and if it is in the list
		if (entity && mChildren.find(entity) != mChildren.end())
		{
			mChildren.erase(entity);
		}
	}

    Ref<Entity> Prefab::CreateChild(Ref<Scene> scene)
    {
        // Create a new entity in the scene
        auto ref = scene->CreateEntity(GetComponent<TagComponent>().Tag);
        auto entity = MakeRef<Entity>(ref);

		// Add the PrefabComponent to the entity
		auto& prefabComponent = entity->AddComponent<PrefabComponent>();
		prefabComponent.mPrefabID = mPrefabID;

		prefabComponent.mParentID = GetUUID();

		// Add all components from the prefab to the new entity
        AddEntityComponent(SpriteRendererComponent);

		// Add the new entity to the list of children
		mChildren.insert(entity);
        return entity;
    }

    //Currently updates manually.
    void Prefab::UpdateAllInstances()
    {
        // Retrieve the list of component names registered in the system
        std::vector<std::string> properties = ComponentRegistry::GetComponentNames();

        for (auto& child : mChildren)
        {
            // Ensure that the child has the PrefabComponent (to track edited properties)
            if (!child->HasComponent<PrefabComponent>())
                continue;

            auto& prefabComp = child->GetComponent<PrefabComponent>();

            //Temp fix
            prefabComp.mEditedComponentList.insert("Transform Component:Translate");
            //template <typename Component>
			//UpdateComponent<Component>(child, prefabComp);
            UpdateComponent<TransformComponent>(child);
            UpdateComponent<SpriteRendererComponent>(child);
            UpdateComponent<CircleRendererComponent>(child);
            UpdateComponent<CameraComponent>(child);
            UpdateComponent<NativeScriptComponent>(child);
            UpdateComponent<MeshFilterComponent>(child);
            UpdateComponent<MeshRendererComponent>(child);
            UpdateComponent<BoxColliderComponent>(child);
            UpdateComponent<CapsuleColliderComponent>(child);
            UpdateComponent<RigidBodyComponent>(child);
            UpdateComponent<LightComponent>(child);
            UpdateComponent<TextComponent>(child);
        }
    }

    //Helper function for update componennt
    template <typename ComponentType>
    void Prefab::UpdateComponent(Ref<Entity> child)
    {
        // Check if both prefab and child have the component
        if (!HasComponent<ComponentType>() || !child->HasComponent<ComponentType>())
            return;

        ComponentType& src = GetComponent<ComponentType>();
        ComponentType& dst = child->GetComponent<ComponentType>();

        rttr::instance srcInstance(src);
        rttr::instance dstInstance(dst);
		std::string componentName = dstInstance.get_type().get_name().to_string();
        auto properties = dstInstance.get_type().get_properties();
       
        // Retrieve the PrefabComponent to check edited properties
        if (!child->HasComponent<PrefabComponent>())
            return;
        auto& prefabComp = child->GetComponent<PrefabComponent>();

        for (const auto& prop : properties)
        {
            // Skip updating if the property has been edited
            if (prop.is_valid() && prop.get_value(srcInstance).is_valid())
            {
                const std::string fullComponentName = componentName + ":" + prop.get_name().to_string();
                if (prefabComp.mEditedComponentList.find(fullComponentName) == prefabComp.mEditedComponentList.end())
                {
                    // Update only if the property was not edited
                    prop.set_value(dstInstance, prop.get_value(srcInstance));
                }
            }
        }
    }

    void Prefab::PrintComponentList()
    {
	    // Retrieve the list of component names
	    std::vector<std::string> properties = ComponentRegistry::GetComponentNames();

	    // Iterate through the vector and print each component name
	    for (const auto& property : properties)
	    {
		    std::cout << property << std::endl;
	    }
    }
	
	    //struct PrefabInstanceComponent
	    //{
     //       UUID ParentID;
		   // std::unordered_set<std::type_info> mEditedComponents;
	    //};
    }