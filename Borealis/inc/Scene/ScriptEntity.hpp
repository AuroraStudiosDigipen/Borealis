/******************************************************************************/
/*!
\file		ScriptEntity.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Declares the Script Entity class for Native Scripts

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCRIPT_ENTITY_HPP
#define SCRIPT_ENTITY_HPP
#include <Scene/Entity.hpp>
namespace Borealis
{
	class ScriptEntity
	{
		friend class Scene;
	public:
		/*!***********************************************************************
			\brief
				Virtual Destructor
		*************************************************************************/
		virtual ~ScriptEntity() {};

		/*!***********************************************************************
			\brief
				Virtual Start function
		*************************************************************************/
		virtual void Start() {};

		/*!***********************************************************************
			\brief
				Virtual Update function
			\param dt
				The delta time
		*************************************************************************/
		virtual void Update(float dt) {};

		/*!***********************************************************************
			\brief
				Virtual End function
		*************************************************************************/
		virtual void End() {};

		/*!***********************************************************************
			\brief
				Gets the component
			\tparam T
				The type of component
			\return
				The component
		*************************************************************************/
		template <typename T>
		T& GetComponent()
		{
			return mEntity.GetComponent<T>();
		}

		/*!***********************************************************************
			\brief
				Checks if the entity has the component
			\tparam T
				The type of component
			\return
				True if the entity has the component
		*************************************************************************/
		template <typename T>
		bool HasComponent()
		{
			return mEntity.HasComponent<T>();
		}

		/*!***********************************************************************
			\brief
				Adds the component to the entity
			\tparam T
				The type of component
			\param component
				The component to add
		*************************************************************************/
		template <typename T>
		void AddComponent(T component)
		{
			mEntity.AddComponent<T>(component);
		}

		/*!***********************************************************************
			\brief
				Removes the component from the entity
			\tparam T
				The type of component
		*************************************************************************/
		template <typename T>
		void RemoveComponent()
		{
			mEntity.RemoveComponent<T>();
		}
	private:
		Entity mEntity;
		
	};

}
#endif