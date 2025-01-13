/******************************************************************************/
/*!
\file		SceneHierarchyPanel.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Declares the class Scene Hierarchy in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef SCENE_HIERARCHY_PANEL_HPP
#define SCENE_HIERARCHY_PANEL_HPP

#include <Scene/scene.hpp>
#include <Scene/Entity.hpp>
#include <Core/core.hpp>

#include <Prefab.hpp>

namespace Borealis {
	class SceneHierarchyPanel
	{
	public:
		/*!***********************************************************************
			\brief
				Constructs the Scene Hierarchy Panel
		*************************************************************************/
		SceneHierarchyPanel() {};

		/*!***********************************************************************
			\brief
				Constructs the Scene Hierarchy Panel with a scene
			\param[in] scene
				The scene to be used
		*************************************************************************/
		SceneHierarchyPanel(const Ref<Scene>& scene);

		/*!***********************************************************************
			\brief
				Sets the context of the Scene Hierarchy Panel
			\param[in] scene
				The scene to be used
		*************************************************************************/
		void SetContext(const Ref<Scene>& scene);

		/*!***********************************************************************
			\brief
				Renders the Scene Hierarchy Panel
		*************************************************************************/
		void ImGuiRender();

		/*!***********************************************************************
			\brief
				Gets the selected entity
			\return
				The selected entity
		*************************************************************************/
		Entity& GetSelectedEntity() { return mSelectedEntity; }

		/*!***********************************************************************
			\brief
				Sets the selected entity
			\param[in] entity
				The entity to be set as selected
		*************************************************************************/
		void SetSelectedEntity(const Entity& entity) { mSelectedEntity = entity; }

		//Multi Select tools
		void PushSelectedEntity(const Entity& entity)
		{
			mSelectedEntities.push_back(entity);
		}

		void ClearSelectedEntities()
		{
			mSelectedEntities.clear();
		}

		void PrintAllSelectedEntities() {
			std::cout << "Selected Entities: ";
			for (const auto& entity : mSelectedEntities) {
				std::cout << "Entity" << " "; // Adjust to match your `Entity` class
			}
			std::cout << std::endl;
		}

		void EnableMultiSelect()
		{
			mMultiSelect = true;
		}

		void DisableMultiSelect()
		{
			mMultiSelect = false;
		}

		bool IsMultiSelect()
		{
			return mMultiSelect;
		}

		std::vector<Entity> GetSelectedEntities() const 
		{
			return mSelectedEntities;
		}

	private:

		/*!***********************************************************************
			\brief
				Draws the entity node
			\param[in] entity
				The entity to be drawn
		*************************************************************************/
		void DrawEntityNode(Entity entity);

		/*!***********************************************************************
			\brief
				Draws the components of the entity
			\param[in] entity
				The entity to be drawn
		*************************************************************************/	
		bool DrawComponents(Entity entity);
		Ref<Scene> mContext;
		Entity mSelectedEntity;
		Entity mSelectedPrefab;

		std::vector<Entity> mSelectedEntities;
		bool mMultiSelect = false;
	};
}

#endif