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
#include <Core/HierarchyLayerManager.hpp>

#include <string>
#include <algorithm>

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

		/*!***********************************************************************
			\brief
				Adds an entity to the list of selected entities in multi-select mode.
			\param[in] entity
				The entity to be added to the selected entities list.
		*************************************************************************/
		void PushSelectedEntity(const Entity& entity)
		{
			mSelectedEntities.push_back(entity);
		}

		/*!***********************************************************************
			\brief
				Clears all selected entities from the multi-select list.
		*************************************************************************/
		void ClearSelectedEntities()
		{
			mSelectedEntities.clear();
		}

		/*!***********************************************************************
			\brief
				Enables multi-select mode, allowing multiple entities to be selected.
		*************************************************************************/
		void EnableMultiSelect()
		{
			mMultiSelect = true;
		}

		/*!***********************************************************************
			\brief
				Disables multi-select mode, restricting selection to a single entity.
		*************************************************************************/
		void DisableMultiSelect()
		{
			mMultiSelect = false;
		}

		/*!***********************************************************************
			\brief
				Checks whether multi-select mode is enabled.
			\return
				True if multi-select is enabled, false otherwise.
		*************************************************************************/
		bool IsMultiSelect()
		{
			return mMultiSelect;
		}

		/*!***********************************************************************
			\brief
				Retrieves a list of all currently selected entities in multi-select mode.
			\return
				A vector containing the selected entities.
		*************************************************************************/
		std::vector<Entity> GetSelectedEntities() const 
		{
			return mSelectedEntities;
		}

		/*!***********************************************************************
			\brief
				Performs a fuzzy match between the search query and the target string.
				Checks if all characters in the query exist in the target string in order.
			\param[in] searchQuery
				The search string provided by the user.
			\param[in] target
				The target string to match against.
			\return
				True if the search query matches the target string, false otherwise.
		*************************************************************************/	
		bool FuzzyMatch(const std::string& searchQuery, const std::string& target)
		{
			std::string lowerQuery = searchQuery;
			std::string lowerTarget = target;

			// Convert both strings to lowercase
			std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c) { return std::tolower(c); });
			std::transform(lowerTarget.begin(), lowerTarget.end(), lowerTarget.begin(), [](unsigned char c) { return std::tolower(c); });

			// Check if all characters in the search query exist in the target string in order
			size_t queryIndex = 0;
			for (size_t targetIndex = 0; targetIndex < lowerTarget.size(); ++targetIndex)
			{
				if (lowerQuery[queryIndex] == lowerTarget[targetIndex])
				{
					queryIndex++;
					if (queryIndex == lowerQuery.size())
						return true; // All characters in the query matched
				}
			}
			return false;
		}
		Ref<Scene>* mEditorScene;
		EditorCamera* editorCamera = nullptr;

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