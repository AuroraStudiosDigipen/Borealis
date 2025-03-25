using System;
using System.Runtime.CompilerServices;

namespace Borealis
{
    internal static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(bool value, out ulong entityID);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Generateulong();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong[] GetAllulongs();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool HasMask(ulong ID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogWarning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogError(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DrawLine(Vector3 start, Vector3 end, Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong CreateEntity(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetFullscreen(bool on);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetMasterVolume(float vol);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetSFXVolume(float vol);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetMusicVolume(float vol);
        #region LayerMask
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LayerMask_LayerToName(int layerNum, out string layerName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LayerMask_NameToLayer(string layerName, out int layerNum);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LayerMask_GetMask(out int mask, params string[] layerNames);

        #endregion

        #region Time
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Time_GetDeltaTime();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Time_GetUnscaledDeltaTime();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Time_SetTimeScale(float scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Time_GetTimeScale();
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

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_FindEntity(string name, out ulong foundID);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetComponent(ulong EntityID, Type type, out object component);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetName(ulong id, out string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetName(ulong id, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetTag(ulong id, out string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTag(ulong id, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetEntitiesFromTag(string tag, out GameObject[] entities);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetEntitiesFromLayer(int layer, out GameObject[] entities);

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
        internal extern static void TransformComponent_GetLocalTranslation(ulong id, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetLocalTranslation(ulong id, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetLocalScale(ulong id, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetLocalScale(ulong id, ref Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetLocalRotation(ulong id, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetLocalRotation(ulong id, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetChildCount(ulong instanceID,  out int count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetChild(ulong instanceID, int index, out ulong childID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetParentID(ulong id, ref ulong parentID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetParentID(ulong id, out ulong parentID);
        #endregion

        #region TextComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void TextComponent_GetText(ulong id, out string text);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void TextComponent_SetText(ulong id, string value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void TextComponent_GetColor(ulong id, out Color color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void TextComponent_SetColor(ulong id, ref Color value);

        #endregion

        #region SpriteRendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong id, out Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong id, ref Color color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetSprite(ulong id, out ulong spriteID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetSprite(ulong id, ref ulong spriteID);
        #endregion
        #region CircleRendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetColor(ulong id, out Color color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetColor(ulong id, ref Color color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetThickness(ulong id, out float thickness);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetThickness(ulong id, ref float thickness);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetFade(ulong id, out float fade);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetFade(ulong id, ref float fade);

        #endregion

        #region MeshRenderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshRendererComponent_GetMaterial(ulong id, out ulong MaterialID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshRendererComponent_SetMaterial(ulong id, ref ulong MaterialID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshRendererComponent_GetEnabled(ulong id, out bool state);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshRendererComponent_SetEnabled(ulong id, ref bool state);
        #endregion

        #region OutlineComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void OutlineComponent_GetEnabled(ulong id, out bool state);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void OutlineComponent_SetEnabled(ulong id, ref bool state);
        #endregion

        #region ColliderComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ColliderComponent_GetBounds(ulong id, out Vector3 center, out Vector3 extents, out Vector3 max, out Vector3 min, out Vector3 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ColliderComponent_UpdateScale(ulong id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ColliderComponent_SetActive(ulong id, ref bool state);
        #endregion

        #region Material
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Material_GetSprite(ulong id, out ulong spriteID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Material_SetSprite(ulong id, ref ulong spriteID);
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
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyComponent_Move(ulong id, ref Vector3 vec);
        #endregion

        #region Physics
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Physics_Raycast(Vector3 origin, Vector3 direction, float maxDistance, int layerMask,
            out ulong entityID, out float distance, out Vector3 normal, out Vector3 point);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Physics_RaycastAll(Vector3 origin, Vector3 direction, float maxDistance, int layerMask, 
            out ulong[] entityID, out float[] distance, out Vector3[] normal, out Vector3[] point);
        #endregion

        #region CharacterController
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CharacterController_Move(ulong id, ref Vector3 motion);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CharacterController_IsGrounded(ulong id, out bool grounded);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CharacterController_Jump(ulong id, ref float speed);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CharacterController_GetLinearVelocity(ulong id, out Vector3 vel);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CharacterController_SetLinearVelocity(ulong id, ref Vector3 vel);

        #endregion

        #region AudioSource
      
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int AudioSource_PlayOneShot(ulong id, byte[] audioID, float[] paramValues, string[] param);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int AudioSource_PlayOneShotLabel(ulong id, byte[] audioID, string[] paramValues, string[] param);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int AudioSource_PlayOneShotPosition(ulong id, byte[] audioID, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_Stop(ulong ID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_IsPlaying(ulong ID, out bool playing);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_StopID(ulong UID, int ID);


        #endregion

        #region AudioListener
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioListener_SetListener(ulong ID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static bool AudioSource_IsChannelPlaying(ulong uiD, int ID);

        #endregion

        #region Animator
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_SetCurrentAnimation(ulong uuid, ulong animation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_GetCurrentAnimation(ulong uuid, out ulong animation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_SetBlend(ulong uuid, float blendValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_GetBlend(ulong uuid, out float blendValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_SetSpeed(ulong uuid, float blendValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_GetSpeed(ulong uuid, out float blendValue);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void AnimatorComponent_SetLooping(ulong uuid, bool blendValue);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_GetLooping(ulong uuid, out bool blendValue);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_SetNextAnimation(ulong uuid, ulong animation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_GetNextAnimation(ulong uuid, out ulong animation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_SwapBlendBuffer(ulong uuid);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_GetAnimationDuration(ulong uuid, out float duration);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimatorComponent_GetCurrentTime(ulong uuid, out float duration);
        #endregion

        #region SceneManager
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        internal extern static void SceneManager_SetActiveScene(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManager_Quit();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManager_SetMainCamera(ulong ID);
        #endregion
        #region AudioClip
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string GetAudioClipName(out byte[] guid);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SetAudioClipName(string name, ref byte[] guid);

        #endregion

        #region ParticleSystem
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetDuration(ulong v, out float duration);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetDuration(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetLooping(ulong v, out bool looping);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetLooping(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartDelay(ulong v, out float delay);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartDelay(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartLifeTime(ulong v, out float lifetime);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartLifeTime(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartSpeed(ulong v, out float speed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartSpeed(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_Get3DStartSizeBool(ulong v, out bool startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_Set3DStartSizeBool(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRandomStartSize(ulong v, out bool startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRandomStartSize(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartSize(ulong v, out Vector3 startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartSize(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartSize2(ulong v, out Vector3 startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartSize2(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_Get3DRandomStartRotation(ulong v, out bool startRotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_Set3DRandomStartRotation(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRandomStartRotation(ulong v, out bool startRotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRandomStartRotation(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartRotation(ulong v, out Vector3 startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartRotation(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartRotation2(ulong v, out Vector3 startSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartRotation2(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRandomStartColor(ulong v, out bool color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRandomStartColor(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartColor(ulong v, out Color startColor2);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartColor(ulong v, ref Color value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetStartColor2(ulong v, out Color startColor2);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetStartColor2(ulong v, ref Color value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetEndColorBool(ulong v, out bool color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetEndColorBool(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetEndColor(ulong v, out Color startColor2);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetEndColor(ulong v, ref Color value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetGravityModifier(ulong v, out float gravity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetGravityModifier(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetSimulationSpeed(ulong v, out float speed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetSimulationSpeed(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetMaxParticles(ulong v, out uint max);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetMaxParticles(ulong v, ref uint value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRateOverTime(ulong v, out float speed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRateOverTime(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetAngle(ulong v, out float angle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetAngle(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRadius(ulong v, out float radius);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRadius(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRadiusThickness(ulong v, out float thickness);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRadiusThickness(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetScale(ulong v, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetScale(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetRotation(ulong v, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetRotation(ulong v, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetBillboard(ulong v, out bool bill);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetBillboard(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetUseNoise(ulong v, out bool noise);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetUseNoise(ulong v, ref bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetNoiseStrength(ulong v, out float noise);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetNoiseStrength(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetNoiseFrequency(ulong v, out float noise);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetNoiseFrequency(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetNoiseScrollSpeed(ulong v, out float noise);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetNoiseScrollSpeed(ulong v, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetTexture(ulong v, out ulong textureID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetTexture(ulong v1, ulong v2);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetShape(ulong v, out int shape);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetShape(ulong v, ref EmitterShape value);

        #endregion
    }
}