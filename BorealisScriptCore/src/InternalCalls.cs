﻿using System;
using System.Runtime.CompilerServices;

namespace Borealis
{
    internal static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong GenerateUUID();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong CreateEntity(string text);

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

        #endregion

        #region GameObject
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveComponent(ulong entityID, Type type);
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
    }
}