/******************************************************************************/
/*!
\file		SceneManager.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Declares the class for Scene

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP
#include <unordered_map>
#include <unordered_set>
#include <Scene/Scene.hpp>
#include <Scene/Serialiser.hpp>
#include <Scene/Entity.hpp>
namespace Borealis
{
	class SceneManager
	{
	public:
		/*!***********************************************************************
			\brief
				Creates a new scene with the given name and path
			\param[in] sceneName
				The name of the scene
			\param[in] scenePath
				The path of the scene
		*************************************************************************/
		static void CreateScene(std::string sceneName, std::string scenePath);

		/*!***********************************************************************
			\brief
				Adds a scene into the scene library
			\param[in] sceneName
				The name of the scene
			\param[in] scenePath
				The path of the scene
		*************************************************************************/
		static void AddScene(std::string sceneName, std::string scenePath);

		/*!***********************************************************************
			\brief
				Removes a scene from the scene library
			\param[in] sceneName
				The name of the scene
		*************************************************************************/
		static void RemoveScene(std::string sceneName);

		/*!***********************************************************************
			\brief
				Sets the active scene
			\param[in] sceneName
				The name of the scene in the scene library
		*************************************************************************/
		static void SetActiveScene(std::string sceneName);

		/*!***********************************************************************
			\brief
				Sets the active scene
			\param[in] scene
				The scene to set as active
		*************************************************************************/
		static void SetActiveScene(Ref<Scene> scene) {mActiveScene = scene;}

		/*!***********************************************************************
			\brief
				Gets the active scene
			\return
				The active scene
		*************************************************************************/
		static Ref<Scene> GetActiveScene() { return mActiveScene; };

		/*!***********************************************************************
			\brief
				Gets an entity
			\param[in] entityID
				The UUID of the entity
			\return
				The entity
		*************************************************************************/
		static Entity GetEntity(uint64_t entityID);

		/*!***********************************************************************
			\brief
				Gets the set of all scene names
			\return
				The set of all scene names
		*************************************************************************/
		static std::unordered_set<std::string> GetSceneNames();

		/*!***********************************************************************
			\brief
				Gets the scene library
			\return
				The scene library
		*************************************************************************/
		static std::unordered_map<std::string, std::string>& GetSceneLibrary() { return mSceneLibrary; }

		/*!***********************************************************************
			\brief
				Clears the scene library
		*************************************************************************/
		static void ClearSceneLibrary() { mSceneLibrary.clear(); }

		/*!***********************************************************************
			\brief
				Saves the active scene
		*************************************************************************/
		static void SaveActiveScene(Serialiser& serialiser);
	private:
		static Ref<Scene> mActiveScene;
		static std::unordered_map<std::string, std::string> mSceneLibrary; // Key: Scene Name, Value: Scene Path
	};
}

#endif