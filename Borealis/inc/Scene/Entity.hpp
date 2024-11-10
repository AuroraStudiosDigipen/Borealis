/******************************************************************************/
/*!
\file		Entity.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Declares the Entity class

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <entt.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Components.hpp>
#include <Physics/PhysicsSystem.hpp>

namespace Borealis
{
	class Entity
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor, initializes the entity to null
		*************************************************************************/
		Entity() : mID(entt::null), mScene(nullptr) {};

		/*!***********************************************************************
			\brief
				Constructor with entity handle and scene pointer
			\param[in] handle
				The entity handle
			\param[in] scene
				The scene pointer
		*************************************************************************/
		Entity(entt::entity handle, Scene* scene);

		/*!***********************************************************************
			\brief
				Copy constructor
			\param[in] other
				The entity to copy
		*************************************************************************/
		Entity(const Entity& other) = default;

		/*!***********************************************************************
			\brief
				Adds a component to the entity
			\tparam T
				The type of component to add
			\tparam Args
				The types of arguments to pass to the component constructor
			\param[in] args
				The arguments to pass to the component constructor
			\return
				The reference to the component added
		*************************************************************************/
		template <typename T, typename ...Args>
		T& AddComponent(Args&&... args)
		{
			T& Component = mScene->mRegistry.emplace<T>(mID, std::forward<Args>(args)...);
			mScene->OnComponentAdded(*this, Component);
			return Component;
		}

		/*!***********************************************************************
			\brief
				Checks if the entity has a component
			\tparam T
				The type of component to check
			\return
				True if the entity has the component, false otherwise
		*************************************************************************/
		template<typename T>
		bool HasComponent()
		{
			return mScene->mRegistry.storage<T>().contains(mID);
		}

		/*!***********************************************************************
			\brief
				Gets the component of the entity
			\tparam T
				The type of component to get
			\return
				The reference to the component
		*************************************************************************/
		template<typename T>
		T& GetComponent()
		{
			return mScene->mRegistry.get<T>(mID);
		}

		/*!***********************************************************************
			\brief
				Removes the component from the entity
			\tparam T
				The type of component to remove
		*************************************************************************/
		template<typename T>
		void RemoveComponent()
		{
			if (typeid(T) == typeid(RigidBodyComponent))
				PhysicsSystem::FreeRigidBody(GetComponent<RigidBodyComponent>());

			mScene->mRegistry.remove<T>(mID);
		}

		/*!***********************************************************************
			\brief
				Adds or replaces a component in the entity
			\tparam T
				The type of component to add or replace
			\tparam Args
				The types of arguments to pass to the component constructor
			\param[in] args
				The arguments to pass to the component constructor
			\return
				The reference to the component added or replaced
		*************************************************************************/
		template <typename T, typename...Args>
		T& AddOrReplaceComponent(Args&& ... args)
		{
			T& Component= mScene->mRegistry.emplace_or_replace<T>(mID, std::forward<Args>(args)...);
			mScene->OnComponentAdded(*this, Component);
			return Component;
		}

		const uint64_t& GetUUID() 
		{
			return GetComponent<IDComponent>().ID;
		}
		
		/*!***********************************************************************
			\brief
				Gets the name of the entity
			\return
				The name of the entity
		*************************************************************************/
		std::string GetName()
		{
			return GetComponent<TagComponent>().Tag;
		}

		/*!***********************************************************************
			\brief
				Checks if the entity is valid
			\return
				True if the entity is valid, false otherwise
		*************************************************************************/
		bool IsValid()
		{
			if (mScene == nullptr)
				return false;
			else
				return mScene->mRegistry.valid(mID);
		}

		bool IsActive()
		{
			bool activeResult = GetComponent<TagComponent>().active;
			if (!activeResult)
			{
				return false;
			}
			else
			{
				UUID parentID = GetComponent<TransformComponent>().ParentID;
				return parentID == 0 ? true : mScene->GetEntityByUUID(parentID).IsActive();
			}
		}

		/*!***********************************************************************
			\brief
				Bool overlaod to check if the entity is null
			\return
				True if the entity is not null, false otherwise
		*************************************************************************/
		operator bool() const { return mID != entt::null; }

		/*!***********************************************************************
			\brief
				Overload to convert the entity to uint32_t
			\return
				The entity as uint32_t
		*************************************************************************/
		operator uint32_t() const { return (uint32_t)mID; }

		/*!***********************************************************************
			\brief
				Overload to compare if two entities are the same
			\param[in] other
				The entity to compare with
			\return
				True if the entities are the same, false otherwise
		*************************************************************************/
		bool operator==(const Entity& other) const
		{
			return mID == other.mID && mScene == other.mScene;
		}

		/*!***********************************************************************
			\brief
				Overload to compare if two entities are not the same
			\param[in] other
				The entity to compare with
			\return
				True if the entities are not the same, false otherwise
		*************************************************************************/
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		/*!***********************************************************************
			\brief
				Overload to convert the entity to entt::entity
			\return
				The entity as entt::entity
		*************************************************************************/
		operator entt::entity() const { return mID; }

	private:
		entt::entity mID; // The entity handle
		Scene* mScene; // The scene pointer
	};

}
#endif