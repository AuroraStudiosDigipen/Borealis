/******************************************************************************/
/*!
\file		HierarchyLayerManager.cpp
\author 	Yeo Jun Jie
\par    	email: yeo.junjie\@digipen.edu
\date   	Jan 20, 2025
\brief		Defines the HierarchicalLayerManager class with its class methods and helper functions

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "HierarchyLayerManager.hpp"
#include "Scene/Entity.hpp"  // Ensure this provides access to entity components
#include "Scene/SceneManager.hpp"

namespace Borealis
{

    void HierarchyLayerManager::LoadEntitiesIntoLayerManager(const Ref<Scene>& scene)
    {
        if (!scene) return; // Safety check
		//std::cout << "Loading entities into layer manager" << std::endl;

        // Clear existing data
        mEntityLayerMap.clear();
        mLayeredEntities.clear();

        std::vector<UUID> zeroLayerEntities; // Store entities with layer 0
        int maxLayer = 0; // Track the highest assigned layer

        // First pass: Process entities with layer > 0
        auto view = scene->GetRegistry().view<IDComponent, TagComponent>();
        for (auto entityHandle : view)
        {
            Entity entity = Entity(entityHandle, scene.get());
            if (entity.HasComponent<IDComponent>() && entity.HasComponent<TagComponent>())
            {
                auto& idComp = entity.GetComponent<IDComponent>();
                auto& tagComp = entity.GetComponent<TagComponent>();

                if (tagComp.mHierarchyLayer > 0)
                {
                    mEntityLayerMap[idComp.ID] = tagComp.mHierarchyLayer;
                    mLayeredEntities.push_back(idComp.ID);
                    maxLayer = std::max(maxLayer, tagComp.mHierarchyLayer); // Update max layer
                }
                else
                {
                    zeroLayerEntities.push_back(idComp.ID); // Store separately
                }
            }
        }

        // Sort non-zero layers first
        SortLayers();

        // Second pass: Assign new layers to entities that had layer 0
        for (const auto& uuid : zeroLayerEntities)
        {
            maxLayer++; // Increment the next available layer
            mEntityLayerMap[uuid] = maxLayer;
            mLayeredEntities.push_back(uuid);

            // Update the entity's actual component so the new layer persists
            Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(uuid);
            if (entity.HasComponent<TagComponent>())
            {
                entity.GetComponent<TagComponent>().mHierarchyLayer = maxLayer;
            }
        }

        // Sort again to maintain correct order
        SortLayers();
    }

    void HierarchyLayerManager::SortLayers()
    {
        std::sort(mLayeredEntities.begin(), mLayeredEntities.end(),
            [this](const UUID& a, const UUID& b)
            {
                return mEntityLayerMap.at(a) < mEntityLayerMap.at(b);
            });

        // Update entity hierarchy layers
        UpdateEntityHierarchyLayers();
    }

    void HierarchyLayerManager::UpdateEntityHierarchyLayers()
    {
        auto activeScene = SceneManager::GetActiveScene();
        if (!activeScene) return;  // Safety check

        for (const auto& uuid : mLayeredEntities)
        {
            Entity entity = activeScene->GetEntityByUUID(uuid);
            if (entity && entity.HasComponent<TagComponent>())
            {
                entity.GetComponent<TagComponent>().mHierarchyLayer = mEntityLayerMap[uuid];
            }
        }
    }

    void HierarchyLayerManager::AddEntity(const UUID& uuid, int layer)
    {
        mEntityLayerMap[uuid] = layer;
        mLayeredEntities.push_back(uuid);
        SortLayers();
    }

    void HierarchyLayerManager::RemoveEntity(const UUID& uuid)
    {
        mEntityLayerMap.erase(uuid);
        auto it = std::find(mLayeredEntities.begin(), mLayeredEntities.end(), uuid);
        if (it != mLayeredEntities.end())
            mLayeredEntities.erase(it);
    }

    void HierarchyLayerManager::MoveEntityUp(const UUID& uuid)
    {
        auto it = mEntityLayerMap.find(uuid);
        if (it != mEntityLayerMap.end())
        {
            int currentLayer = it->second;
			if (currentLayer == 1) return; // Highest layer, cannot move up

            // Find entity that currently occupies the target layer
            UUID entityAbove = UUID(); // Default null/empty
            for (auto& [id, layer] : mEntityLayerMap)
            {
                if (layer == currentLayer - 1)
                {
                    entityAbove = id;
                    break;
                }
            }

            if (entityAbove)
            {
                // Swap layer values
                mEntityLayerMap[uuid] = currentLayer - 1;
                mEntityLayerMap[entityAbove] = currentLayer;
            }

            SortLayers();
        }
    }

    void HierarchyLayerManager::MoveEntityDown(const UUID& uuid)
    {
        auto it = mEntityLayerMap.find(uuid);
        if (it == mEntityLayerMap.end()) return; // Entity not found

        int currentLayer = it->second;

        // Find entity in the layer below
        UUID entityBelow = UUID();
        for (auto& [id, layer] : mEntityLayerMap)
        {
            if (layer == currentLayer + 1)
            {
                entityBelow = id;
                break;
            }
        }

        // If no entity is found below, stay in place
        if (!entityBelow) return;

        // Swap layers
        mEntityLayerMap[uuid] = currentLayer + 1;
        mEntityLayerMap[entityBelow] = currentLayer;

        SortLayers();
    }

    std::vector<UUID> HierarchyLayerManager::GetEntitiesInLayerOrder() const
    {
        return mLayeredEntities;
    }

    std::unordered_map<UUID, int> HierarchyLayerManager::GetEntityLayerMap() const
    {
		return mEntityLayerMap;
    }
}
