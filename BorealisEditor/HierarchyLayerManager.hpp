#pragma once
#include <map>
#include <Scene/Entity.hpp>
#include <Core/Core.hpp>
#include <entt.hpp>
#include <yaml-cpp/yaml.h>

namespace Borealis
{
    class HierarchyLayerManager
    {
    public:
        // Public method to access the singleton instance
        static HierarchyLayerManager& GetInstance()
        {
            static HierarchyLayerManager instance;
            return instance;
        }

        // Delete copy constructor and assignment operator to ensure the singleton cannot be copied
        HierarchyLayerManager(const HierarchyLayerManager&) = delete;
        HierarchyLayerManager& operator=(const HierarchyLayerManager&) = delete;

        // Public methods
        void AddEntityToMap(Entity entity)
        {
            //mHLayerMap[layer] = entity;
        }

    private:
        // Private constructor
        HierarchyLayerManager() = default;

        // Member variables
        std::map<int, Entity> mHLayerMap; // Layer -> Entity
    };
}
