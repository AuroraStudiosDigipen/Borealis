/******************************************************************************/
/*!
\file		HierarchyLayerManager.hpp
\author 	Yeo Jun Jie
\par    	email: yeo.junjie\@digipen.edu
\date   	Jan 20, 2025
\brief		Declares the HierarchicalLayerManager class with its class methods and helper functions

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <Scene/Entity.hpp>
#include <Core/Core.hpp>
#include <Scene/SceneManager.hpp>

namespace Borealis
{
	class HierarchyLayerManager
	{
	public:
		// Singleton access method
		static HierarchyLayerManager& GetInstance()
		{
			static HierarchyLayerManager instance;
			return instance;
		}

		// Delete copy constructor and assignment operator
		HierarchyLayerManager(const HierarchyLayerManager&) = delete;
		HierarchyLayerManager& operator=(const HierarchyLayerManager&) = delete;
		
		//Loading of Map
		void LoadEntitiesIntoLayerManager(const Ref<Scene>& scene);

		// Methods to manage layers
		void AddEntity(const UUID& uuid, int layer);
		void RemoveEntity(const UUID& uuid);
		void MoveEntityUp(const UUID& uuid);
		void MoveEntityDown(const UUID& uuid);
		std::vector<UUID> GetEntitiesInLayerOrder() const;
		std::unordered_map<UUID, int> GetEntityLayerMap() const;

	private:
		// Private a
		HierarchyLayerManager() = default;

		// Layer tracking structures
		std::unordered_map<UUID, int> mEntityLayerMap; // Maps UUIDs to their layers
		std::vector<UUID> mLayeredEntities;           // Stores UUIDs in the order they should be drawn

		// Helper methods
		void SortLayers();
		void UpdateEntityHierarchyLayers();

	};
}
