#include "HierarchyLayerManager.hpp"
#include <Scene/SceneManager.hpp>
#include <iostream>

namespace Borealis
{
    // Remove an entity from layer tracking
    void HierarchyLayerManager::RemoveEntity(Entity entity)
    {
        auto it = mEntityLayerMap.find(entity);
        if (it != mEntityLayerMap.end())
        {
            int layer = it->second;
            mEntityLayerMap.erase(it);
            mLayerToEntityMap.erase(layer);
            mLayeredEntities.erase(std::remove(mLayeredEntities.begin(), mLayeredEntities.end(), entity), mLayeredEntities.end());
        }
    }

    // Move entity up in layer order (swap with the one above if it exists)
    void HierarchyLayerManager::MoveEntityUp(Entity entity)
    {
        auto it = mEntityLayerMap.find(entity);
        if (it != mEntityLayerMap.end())
        {
            int currentLayer = it->second;
            int newLayer = currentLayer + 1;

            if (mLayerToEntityMap.find(newLayer) != mLayerToEntityMap.end())
            {
                Entity swapEntity = mLayerToEntityMap[newLayer];
                mEntityLayerMap[swapEntity] = currentLayer;
                mLayerToEntityMap[currentLayer] = swapEntity;
            }

            mEntityLayerMap[entity] = newLayer;
            mLayerToEntityMap[newLayer] = entity;
        }
    }

    // Move entity down in layer order (swap with the one below if it exists)
    void HierarchyLayerManager::MoveEntityDown(Entity entity)
    {
        auto it = mEntityLayerMap.find(entity);
        if (it != mEntityLayerMap.end())
        {
            int currentLayer = it->second;
            int newLayer = currentLayer - 1;

            if (mLayerToEntityMap.find(newLayer) != mLayerToEntityMap.end())
            {
                Entity swapEntity = mLayerToEntityMap[newLayer];
                mEntityLayerMap[swapEntity] = currentLayer;
                mLayerToEntityMap[currentLayer] = swapEntity;
            }

            mEntityLayerMap[entity] = newLayer;
            mLayerToEntityMap[newLayer] = entity;
        }
    }

    // Get entities in the order they should be drawn
    std::vector<Entity> HierarchyLayerManager::GetEntitiesInLayerOrder() const
    {
        return mLayeredEntities;
    }

    // Load all entities into the layer manager from the scene
    void HierarchyLayerManager::LoadEntitiesIntoLayerManager(const Ref<Scene>& scene)
    {
        mLayeredEntities.clear();
        mEntityLayerMap.clear();
        mLayerToEntityMap.clear();

        for (auto& item : scene->GetRegistry().view<entt::entity>())
        {
            Entity entity{ item, scene.get() };

            // Assume each entity has a component that defines its layer (e.g., TagComponent)
            if (entity.HasComponent<TagComponent>())
            {
                auto& tagComponent = entity.GetComponent<TagComponent>();

                int layer = tagComponent.mLayer.to_ulong(); // Convert bitset to int
                if (layer == 0)
                {
                    layer = GetNextAvailableLayer();
                }

                mEntityLayerMap[entity] = layer;
                mLayerToEntityMap[layer] = entity;
                mLayeredEntities.push_back(entity);
            }
        }

        SortEntitiesByLayer();
    }

    // Get the next available layer ID
    int HierarchyLayerManager::GetNextAvailableLayer()
    {
        int layer = 1;
        while (mLayerToEntityMap.find(layer) != mLayerToEntityMap.end())
        {
            layer++;
        }
        return layer;
    }

    // Sort entities based on layer values
    void HierarchyLayerManager::SortEntitiesByLayer()
    {
        std::sort(mLayeredEntities.begin(), mLayeredEntities.end(),
            [this](Entity a, Entity b)
            {
                return mEntityLayerMap[a] < mEntityLayerMap[b];
            });
    }
}
