/******************************************************************************/
/*!
\file		Scene.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Declares the class for Scene

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCENE_HPP
#define SCENE_HPP
#include <entt.hpp>
#include <Core/UUID.hpp>
#include <Graphics/EditorCamera.hpp>
#include <Graphics/RenderGraph/RenderGraph.hpp>
namespace Borealis
{
	class Entity;

	class Scene
	{
	public:

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class Serialiser;

		/*!***********************************************************************
			\brief
				Default Constructor for Scene
			\param[in] name
				The name of the scene, if any
			\param[in] scenepath
				The path of the scene, if any
		*************************************************************************/
		Scene(std::string name = "newScene", std::string scenepath = "");

		/*!***********************************************************************
			\brief
				Virtual Destructor for Scene
		*************************************************************************/
		virtual ~Scene();

		/*!***********************************************************************
			\brief
				Copies a Scene
			\param[in] other
				The scene to copy
			\return
				A reference to the copied scene
		*************************************************************************/
		static Ref<Scene> Copy(const Ref<Scene>& other);

		/*!***********************************************************************
			\brief
				Runtime update function
			\param[in] dt
				The delta time
		*************************************************************************/
		void UpdateRuntime(float dt);

		void Render2DPass();
		void Render3DPass();

		/*!***********************************************************************
			\brief
				Editor update function
			\param[in] dt
				The delta time
			\param[in] camera
				The editor camera
		*************************************************************************/
		void UpdateEditor(float dt, EditorCamera& camera);

		/*!***********************************************************************
			\brief
				Creates an entity in the scene
			\param[in] name
				The name of the entity
		*************************************************************************/
		Entity CreateEntity(const std::string& name);

		/*!***********************************************************************
			\brief
				Creates an entity with a UUID
			\param[in] name
				The name of the entity
			\param[in] UUID
				The UUID of the entity
		*************************************************************************/
		Entity CreateEntityWithUUID(const std::string& name, uint64_t UUID);

		/*!***********************************************************************
			\brief
				Gets an entity by its UUID
			\param[in] uuid
				The UUID of the entity
		*************************************************************************/
		Entity GetEntityByUUID(UUID uuid);

		/*!***********************************************************************
			\brief
				Deletes an entity from the scene
			\param[in] entity
				The entity to delete
		*************************************************************************/
		void DestroyEntity(Entity entity);

		/*!***********************************************************************
			\brief
				Deletes an entity from the scene
			\param[in] entity
				The entity to delete
		*************************************************************************/
		void DuplicateEntity(Entity entity);

		/*!***********************************************************************
			\brief
				Resizes the viewport
			\param[in] width
				The width of the viewport
			\param[in] height
				The height of the viewport
		*************************************************************************/
		void ResizeViewport(const uint32_t& width, const uint32_t& height);

		/*!***********************************************************************
			\brief
				Getter for the registry
			\return
				The registry
		*************************************************************************/
		entt::registry& GetRegistry() { return mRegistry; }

		/*!***********************************************************************
			\brief
				Getter for the name of the scene
			\return
				The name of the scene
		*************************************************************************/
		const std::string& GetName() const { return mName; }

		/*!***********************************************************************
			\brief
				Getter for the path of the scene
			\return
				The path of the scene
		*************************************************************************/
		const std::string& GetScenePath() const { return mScenePath; }

		/*!***********************************************************************
			\brief
				Setter for the name of the scene
			\return
				The name of the scene
		*************************************************************************/
		void SetName(const std::string& name) { mName = name; }

		/*!***********************************************************************
			\brief
				Setter for the path of the scene
			\return
				The path of the scene
		*************************************************************************/
		void SetScenePath(const std::string& path) { mScenePath = path; }

		/*!***********************************************************************
			\brief
				Starts the Runtime
		*************************************************************************/
		void RuntimeStart();

		/*!***********************************************************************
			\brief
				Ends the Runtime
		*************************************************************************/
		void RuntimeEnd();

		/*!***********************************************************************
			\brief
				Getter for the primary camera entity
			\return
				The primary camera entity
		*************************************************************************/
		Entity GetPrimaryCameraEntity();

		Ref<FrameBuffer> GetRunTimeFB();
		Ref<FrameBuffer> GetEditorFB();

		void SetRenderGraphConfig(RenderGraphConfig renderGraphConfig);

		void AddRenderGraphGlobalSource(Ref<RenderSource> globalSource);
	private:

		/*!***********************************************************************
			\brief
				Template function to run when any component is added
			\param[in] entity
				The entity
			\param[in] component
				The component
		*************************************************************************/
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		entt::registry mRegistry;
		uint32_t mViewportWidth = 0, mViewportHeight = 0;
		std::string mName;
		std::string mScenePath;
		std::unordered_map<UUID, entt::entity> mEntityMap;

		bool hasRuntimeStarted = false;

		Ref<FrameBuffer> mViewportFrameBuffer;
		Ref<FrameBuffer> mRuntimeFrameBuffer;
		Ref<FrameBuffer> mGFrameBuffer;
		RenderGraph mRenderGraph;
	};
}

#endif