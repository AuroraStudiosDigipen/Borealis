using System;
using System.Runtime.InteropServices;
namespace Borealis
{
    public struct RaycastHit
    {
        public Transform Transform;
        public float distance;
    }
    public class Physics
    {
        public const int DefaultRaycastLayers = 1;

        public static RaycastHit[] RaycastAll(Ray ray, float maxDistance, int layerMask = DefaultRaycastLayers)
        {
            // Get all game objects and loop through
            GameObject[] gameObjects = GameObject.FindObjectsWithLayerMask(layerMask);
            RaycastHit[] hits = new RaycastHit[gameObjects.Length];
            for (int i = 0; i < gameObjects.Length; i++)
            {
                if (gameObjects[i].HasComponent<Rigidbody>())
                {
                    //Rigidbody body = gameObjects[i].GetComponent<Rigidbody>();
                    //Vector3 Position = body.position;
                    //Vector3 Rotation = body.rotation;
                    //Vector3 Size = body.Size;
                    if (true)
                    {
                        hits[i] = new RaycastHit();
                        hits[i].distance = 0;
                    }

                }


                // For now, just add a placeholder
            }
            return hits;
        }
    }
}
