using System;
using System.Runtime.InteropServices;
namespace Borealis
{
    public struct RaycastHit
    {
        public Transform transform;
        public float distance;
        public Vector3 normal;
        public Vector3 point;
    }
    public class Physics
    {
        public const int DefaultRaycastLayers = 1;

        public static bool Raycast(Vector3 origin, Vector3 direction, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return InternalCalls.Physics_Raycast(origin, direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
        }

        public static bool Raycast(Vector3 origin, Vector3 direction, out RaycastHit hitInfo, float maxDistance, int layerMask)
        {
            bool output = InternalCalls.Physics_Raycast(origin, direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
            if (output)
            {
                hitInfo.distance = distance;
                hitInfo.normal = normal;
                hitInfo.point = point;
                hitInfo.transform = new Transform(entityID);
            }
            else
            {
                hitInfo.distance = 0;
                hitInfo.normal = Vector3.zero;
                hitInfo.point = Vector3.zero;
                hitInfo.transform = new Transform(0);
            }
            return output;
        }

        public static bool Raycast(Ray ray, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return InternalCalls.Physics_Raycast(ray.origin, ray.direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
        }


        public static bool Raycast(Ray ray, out RaycastHit hitInfo, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            bool output = InternalCalls.Physics_Raycast(ray.origin, ray.direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
            if (output)
            { 
                hitInfo.distance = distance;
                hitInfo.normal = normal;
                hitInfo.point = point;
                hitInfo.transform = new Transform(entityID);
            }
            else
            {
                hitInfo.distance = 0;
                hitInfo.normal = Vector3.zero;
                hitInfo.point = Vector3.zero;
                hitInfo.transform = new Transform(0);
            }

            return output;
        }

        public static RaycastHit[] RaycastAll(Vector3 origin, Vector3 direction, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            InternalCalls.Physics_RaycastAll(origin, direction, maxDistance, layerMask,
                out ulong[] entityID, out float[] distance, out Vector3[] normal, out Vector3[] point);

            RaycastHit[] hitInfos = new RaycastHit[entityID.Length];
            for (int i = 0; i < entityID.Length; i++)
            {
                RaycastHit hitInfo = new RaycastHit();
                hitInfo.distance = distance[i];
                hitInfo.normal = normal[i];
                hitInfo.point = point[i];
                hitInfo.transform = new Transform(entityID[i]);
                hitInfos[i] = hitInfo;
            }
            return hitInfos;
        }

        public static RaycastHit[] RaycastAll(Ray ray, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            InternalCalls.Physics_RaycastAll(ray.origin, ray.direction, maxDistance, layerMask,
                            out ulong[] entityID, out float[] distance, out Vector3[] normal, out Vector3[] point);

            RaycastHit[] hitInfos = new RaycastHit[entityID.Length];
            for (int i = 0; i < entityID.Length; i++)
            {
                RaycastHit hitInfo = new RaycastHit();
                hitInfo.distance = distance[i];
                hitInfo.normal = normal[i];
                hitInfo.point = point[i];
                hitInfo.transform = new Transform(entityID[i]);
                hitInfos[i] = hitInfo;
            }
            return hitInfos;
        }
    }
}
