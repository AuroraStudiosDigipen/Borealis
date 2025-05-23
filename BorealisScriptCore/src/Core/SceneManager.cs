﻿
namespace Borealis
{
    public class SceneManager
    {
        public static void SetActiveScene(string name)
        {
            InternalCalls.SceneManager_SetActiveScene(name);
        }

        public static void Quit()
        {
            InternalCalls.SceneManager_Quit();
        }

        public static void SetMainCamera(GameObject ob)
        {
            InternalCalls.SceneManager_SetMainCamera(ob.GetInstanceID());
        }

        public static void SetGamma(float gamma)
        {
            InternalCalls.SceneManager_SetGamma(gamma);
        }
    }
}
