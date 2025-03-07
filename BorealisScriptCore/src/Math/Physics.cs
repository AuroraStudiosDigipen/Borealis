﻿using System;
using System.Runtime.InteropServices;
namespace Borealis
{
    public struct RaycastHit
    {
        public Transform transform;
        public float distance;
        public Vector3 normal;
        public Vector3 point;
        public Collider collider;
    }
    public class Physics
    {
        public const int DefaultRaycastLayers = 0;

        public static bool Raycast(Vector3 origin, Vector3 direction, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return InternalCalls.Physics_Raycast(origin, direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
        }

        public static bool Raycast(Vector3 origin, Vector3 direction, out RaycastHit hitInfo, float maxDistance, int layerMask)
        {
            bool output = InternalCalls.Physics_Raycast(origin, direction, maxDistance, layerMask, out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);
            if (output)
            {
                hitInfo = new RaycastHit();
                hitInfo.distance = distance;
                hitInfo.normal = normal;
                hitInfo.point = point;
                hitInfo.transform = new Transform(entityID);
                hitInfo.collider = new Collider(entityID);
            }
            else
            {
                hitInfo = new RaycastHit();
                hitInfo.distance = 0;
                hitInfo.normal = Vector3.zero;
                hitInfo.point = Vector3.zero;
                hitInfo.transform = null;
                hitInfo.collider = null;
            }
            return output;
        }

        public static bool Raycast(Ray ray, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return Raycast(ray.origin, ray.direction, maxDistance, layerMask);
        }


        public static bool Raycast(Ray ray, out RaycastHit hitInfo, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return Raycast(ray.origin, ray.direction, out hitInfo, maxDistance, layerMask);
        }

        public static RaycastHit[] RaycastAll(Vector3 origin, Vector3 direction, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            InternalCalls.Physics_RaycastAll(origin, direction, maxDistance, layerMask,
                             out ulong[] entityID, out float[] distance, out Vector3[] normal, out Vector3[] point);

            if (distance == null)
            {
                return new RaycastHit[0];
            }
            RaycastHit[] hitInfos = new RaycastHit[distance.Length];

            for (int i = 0; i < distance.Length; i++)
            {
                RaycastHit hitInfo = new RaycastHit();
                hitInfo.distance = distance[i];
                hitInfo.transform = new Transform(entityID[i]);
                hitInfo.normal = normal[i];
                hitInfo.point = point[i];
                hitInfos[i] = hitInfo;
                hitInfo.collider = new Collider(entityID[i]);

            }

            return hitInfos;
        }

        public static RaycastHit[] RaycastAll(Ray ray, float maxDistance = Mathf.Infinity, int layerMask = DefaultRaycastLayers)
        {
            return RaycastAll(ray.origin, ray.direction, maxDistance, layerMask);
        }
    }
}
