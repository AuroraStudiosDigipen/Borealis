
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
    }
}
