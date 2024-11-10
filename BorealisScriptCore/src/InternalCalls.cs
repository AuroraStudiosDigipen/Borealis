using System;
using System.Runtime.CompilerServices;

namespace Borealis
{
    internal static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(bool value, out ulong entityID);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong GenerateUUID();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong[] GetAllUUIDs();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool HasMask(ulong ID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogWarning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogError(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong CreateEntity(string text);
        
        #region Time
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Time_GetDeltaTime();
        #endregion

        #region Input

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_AnyKey(out bool key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_AnyKeyDown(out bool key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Vector3 Input_GetMousePosition();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Vector3 Input_GetMouseScrollDelta();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKey(int key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKeyDown(int key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKeyUp(int key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouse(int mouse);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseDown(int mouse);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseUp(int mouse);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Input_GetAxis(string axis);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Input_GetAxisRaw(string axis);

        #endregion

        #region Cursor
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Cursor_GetVisibility(out bool axis);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Cursor_SetVisibility(ref bool axis);
        #endregion

        #region GameObject
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetActive(ulong id, out bool active);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetActive(ulong id, ref bool active);
        #endregion

        #region TransformComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong id, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong id, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong id, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong id, ref Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong id, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong id, ref Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetParentID(ulong id, ref ulong parentID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetParentID(ulong id, out ulong parentID);
        #endregion

        #region SpriteRendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong id, out Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong id, ref Color color);
        #endregion

        #region ScriptComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_AddComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool ScriptComponent_HasComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_RemoveComponent(ulong entityID, Type type);
        #endregion


        #region RigidBodyComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetLinearVelocity(ulong id, out Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetLinearVelocity(ulong id, ref Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetAngularVelocity(ulong id, out Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetAngularVelocity(ulong id, ref Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetPosition(ulong id, out Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetPosition(ulong id, ref Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetRotation(ulong id, out Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetRotation(ulong id, ref Vector3 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetMass(ulong id, out float mass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetMass(ulong id, ref float mass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetUseGravity(ulong id, out bool useGravity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetUseGravity(ulong id, ref bool useGravity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_GetIsKinematic(ulong id, out bool isKinematic);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_SetIsKinematic(ulong id, ref bool isKinematic);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_AddForce(ulong id, ref Vector3 force);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_AddTorque(ulong id, ref Vector3 force);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_AddImpulse(ulong id, ref Vector3 force);


        #endregion
    }
}