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
        }
        mPrefabID = entity.GetComponent<PrefabComponent>().mPrefabID;
		// Add all components to prefab manager ECS

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


		// Add all components from the prefab to the new entity
        AddEntityComponent(SpriteRendererComponent);

		// Add the new entity to the list of children
		mChildren.insert(entity);
        return entity;
    }

//void Prefab::UpdateAllInstances()
//{
//    for (auto& child : mChildren)
//    {
//        // Dereference the smart pointer to access the child entity
//        if (child->HasComponent<SpriteRendererComponent>())
//        {
//			auto prefabComp = child->GetComponent<PrefabComponent>(); //All child must have this component
//            // Add or replace the SpriteRendererComponent in the child entity
//			for (auto variableName : ComponentRegistry::GetPropertyNames("SpriteRendererComponent"))
//			{
//				if (prefabComp.mEditedComponentList.contains(std::string("SpriteRendererComponent") + "::" + variableName))
//				{
//					// Cout	
//					//child->thisvariablename = parent's->thisvariablename
//				}
//			}
//        }
//    }
//}

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

            // Loop through each component name dynamically
            for (const auto& componentName : properties)
            {
                if (componentName == "SpriteRendererComponent")
                {
                    // Check if the child has this component
                    if (!child->HasComponent<SpriteRendererComponent>())
                        continue; // Child doesn't have this component, skip

                    // Retrieve the component from both the prefab and the child
                    auto& childComponent = child->GetComponent<SpriteRendererComponent>();
                    auto& prefabComponent = PrefabManager::GetRegistry().get<SpriteRendererComponent>(mPrefabID);

                    // Get the property names of this component dynamically
                    std::vector<std::string> variableNames = ComponentRegistry::GetPropertyNames("SpriteRendererComponent");

                    // Copy properties from the prefab to the child unless the child has overridden them
                    for (const auto& variableName : variableNames)
                    {
                        // Construct a full property identifier like "SpriteRendererComponent::Colour"
                        std::string fullPropertyName = "SpriteRendererComponent::" + variableName;

                        // Check if this property has been edited in the child (skip it if it has)
                        if (prefabComp.mEditedComponentList.contains(fullPropertyName))
                            continue; // Property was edited in the child, don't overwrite it

                        // Use the CopyPropertyValue function to copy from prefab to child
                        ComponentRegistry::CopyPropertyValue(variableName, prefabComponent, childComponent);
                    }
                }
                else if (componentName == "TransformComponent")
                {
                    // Check if the child has this component
                    if (!child->HasComponent<TransformComponent>())
                        continue; // Child doesn't have this component, skip

                    // Retrieve the component from both the prefab and the child
                    auto& childComponent = child->GetComponent<TransformComponent>();
                    auto& prefabComponent = PrefabManager::GetRegistry().get<TransformComponent>(mPrefabID);

                    // Get the property names of this component dynamically
                    std::vector<std::string> variableNames = ComponentRegistry::GetPropertyNames("TransformComponent");

                    // Copy properties from the prefab to the child unless the child has overridden them
                    for (const auto& variableName : variableNames)
                    {
                        // Construct a full property identifier like "TransformComponent::Translate"
                        std::string fullPropertyName = "TransformComponent::" + variableName;

                        // Check if this property has been edited in the child (skip it if it has)
                        if (prefabComp.mEditedComponentList.contains(fullPropertyName))
                            continue; // Property was edited in the child, don't overwrite it

                        // Use the CopyPropertyValue function to copy from prefab to child
                        ComponentRegistry::CopyPropertyValue(variableName, prefabComponent, childComponent);
                    }
                }
                else if (componentName == "CircleRendererComponent")
                {
                    // Check if the child has this component
                    if (!child->HasComponent<CircleRendererComponent>())
                        continue; // Child doesn't have this component, skip

                    // Retrieve the component from both the prefab and the child
                    auto& childComponent = child->GetComponent<CircleRendererComponent>();
                    auto& prefabComponent = PrefabManager::GetRegistry().get<CircleRendererComponent>(mPrefabID);

                    // Get the property names of this component dynamically
                    std::vector<std::string> variableNames = ComponentRegistry::GetPropertyNames("CircleRendererComponent");

                    // Copy properties from the prefab to the child unless the child has overridden them
                    for (const auto& variableName : variableNames)
                    {
                        // Construct a full property identifier like "TransformComponent::Translate"
                        std::string fullPropertyName = "CircleRendererComponent::" + variableName;

                        // Check if this property has been edited in the child (skip it if it has)
                        if (prefabComp.mEditedComponentList.contains(fullPropertyName))
                            continue; // Property was edited in the child, don't overwrite it

                        // Use the CopyPropertyValue function to copy from prefab to child
                        ComponentRegistry::CopyPropertyValue(variableName, prefabComponent, childComponent);
                    }
                }
                else if (componentName == "CameraComponent")
                {
                    // Check if the child has this component
                    if (!child->HasComponent<CircleRendererComponent>())
                        continue; // Child doesn't have this component, skip

                    // Retrieve the component from both the prefab and the child
                    auto& childComponent = child->GetComponent<CameraComponent>();
                    auto& prefabComponent = PrefabManager::GetRegistry().get<CameraComponent>(mPrefabID);

                    // Get the property names of this component dynamically
                    std::vector<std::string> variableNames = ComponentRegistry::GetPropertyNames("CameraComponent");

                    // Copy properties from the prefab to the child unless the child has overridden them
                    for (const auto& variableName : variableNames)
                    {
                        // Construct a full property identifier like "TransformComponent::Translate"
                        std::string fullPropertyName = "CameraComponent::" + variableName;

                        // Check if this property has been edited in the child (skip it if it has)
                        if (prefabComp.mEditedComponentList.contains(fullPropertyName))
                            continue; // Property was edited in the child, don't overwrite it

                        // Use the CopyPropertyValue function to copy from prefab to child
                        ComponentRegistry::CopyPropertyValue(variableName, prefabComponent, childComponent);
                    }
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
	
	struct PrefabInstanceComponent
	{
		std::unordered_set<std::type_info> mEditedComponents;
		//HasComponentEdited
	};
}