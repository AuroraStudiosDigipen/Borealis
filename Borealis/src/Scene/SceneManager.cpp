#include "BorealisPCH.hpp"
#include "Scene/SceneManager.hpp"
#include <Scene/Serialiser.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/Core.hpp>

namespace Borealis
{
    Ref<Scene> SceneManager::mActiveScene;
    std::unordered_map<std::string, std::string> SceneManager::mSceneLibrary;
    void SceneManager::AddScene(std::string sceneName, std::string scenePath)
    {
        std::transform(sceneName.begin(), sceneName.end(), sceneName.begin(), ::tolower);
        if (mSceneLibrary.find(sceneName) == mSceneLibrary.end())
            mSceneLibrary[sceneName] = scenePath;
        else
            BOREALIS_CORE_ERROR("Scene {0} already exists in Scene Library", sceneName);
    }
    void SceneManager::RemoveScene(std::string sceneName)
    {
        std::transform(sceneName.begin(), sceneName.end(), sceneName.begin(), ::tolower);
        if (mSceneLibrary.find(sceneName) != mSceneLibrary.end())
            mSceneLibrary.erase(sceneName);
        else
            BOREALIS_CORE_ERROR("Scene {0} not found in Scene Library", sceneName);
    }
    void SceneManager::SetActiveScene(std::string sceneName)
    {
		std::transform(sceneName.begin(), sceneName.end(), sceneName.begin(), ::tolower);
        if (mSceneLibrary.find(sceneName) != mSceneLibrary.end())
        {
            Ref<Scene> newScene = MakeRef<Scene>(sceneName, mSceneLibrary[sceneName]);
            if (mSceneLibrary[sceneName] != "")
            {
                Serialiser serialiser(newScene);
                serialiser.DeserialiseScene(SceneManager::GetSceneLibrary()[sceneName]);
            }
            mActiveScene = newScene;
		}
		else
			BOREALIS_CORE_ERROR("Scene {0} not found in Scene Library", sceneName);
    }
    Entity SceneManager::GetEntity(uint64_t entityID)
    {
        return mActiveScene->GetEntityByUUID(entityID);
    }
    std::unordered_set<std::string> SceneManager::GetSceneNames()
    {
        std::unordered_set<std::string> sceneNames;
		for (auto& scene : mSceneLibrary)
			sceneNames.insert(scene.first);
		return sceneNames;
    }
}