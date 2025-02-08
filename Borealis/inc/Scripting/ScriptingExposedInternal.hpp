/******************************************************************************/
/*!
\file		ScriptingExposedInternal.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 13, 2024
\brief		Declares the functions to expose to C# Scripting Environment

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#ifndef SCRIPTING_EXPOSED_INTERNAL_HPP
#define SCRIPTING_EXPOSED_INTERNAL_HPP
#include <glm/glm.hpp>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <Scene/Entity.hpp>
#include <Core/UUID.hpp>
namespace Borealis
{
	// Data structures
	using HasComponentFn = std::function<bool(Entity&)>;
	using AddComponentFn = std::function<void(Entity&)>;
	using RemoveComponentFn = std::function<void(Entity&)>;

	typedef struct GenericComponentFunctionMaps
	{
		static std::unordered_map<std::string, HasComponentFn> mHasComponentFunctions;
		static std::unordered_map<std::string, AddComponentFn> mAddComponentFunctions;
		static std::unordered_map<std::string, RemoveComponentFn> mRemoveComponentFunctions;
	} GCFM;

	/*!***********************************************************************
		\brief
			Registers all the internal functions that are written in this
			file
	*************************************************************************/
	void RegisterInternals();
	
	/*!***********************************************************************
		\brief
			Generates a UUID
		\return
			uint64_t of the generated UUID
	*************************************************************************/
	uint64_t GenerateUUID();

	/*!***********************************************************************
		\brief
			Logs a text to the console
		\param[in] text
			Text to log
	*************************************************************************/
	void Log(MonoString* text);
	void LogWarning(MonoString* text);
	void LogError(MonoString* text);
	void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);

	void SetActive(bool value, uint64_t* outEntityID);

	/*!***********************************************************************
		\brief
			Creates an Entity
		\param[in] name
			Name of the Entity
		\return 
			uint64_t of the Entity ID
	*************************************************************************/
	uint64_t CreateEntity(MonoString* name);

	/*!***********************************************************************
		\brief
			Removes a Component from an Entity
		\param[in] entityID
			Entity ID to remove the component from
		\param[in] reflectionType
			Type of the component to remove
	*************************************************************************/
	void Entity_RemoveComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	/*!***********************************************************************
		\brief
			Add a Component from an Entity
		\param[in] entityID
			Entity ID to add the component from
		\param[in] reflectionType
			Type of the component to add
	*************************************************************************/
	void Entity_AddComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	/*!***********************************************************************
		\brief
			Checks if an Entity has a specific component
		\param[in] entityID
			Entity ID to check the component from
		\param[in] reflectionType
			Type of the component to check
		\return
			True if has, false if not
	*************************************************************************/
	bool Entity_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	void Entity_GetComponent(uint64_t entityID, MonoReflectionType* reflectionType, MonoObject** component);

	void Entity_SetActive(uint64_t entityID, bool* active);
	void Entity_GetActive(uint64_t entityID, bool* active);

	void Entity_GetName(uint64_t entityID, MonoString** name);
	void Entity_SetName(uint64_t entityID, MonoString* name);
	void Entity_GetTag(uint64_t entityID, MonoString** tag);
	void Entity_SetTag(uint64_t entityID, MonoString* tag);

	void Entity_FindEntity(MonoString* name, UUID* ID);

	void Entity_GetEntitiesFromLayer(int32_t layerNum, MonoArray** objectArray);
	void Entity_GetEntitiesFromTag(MonoString* layerNum, MonoArray** objectArray);
	float Time_GetDeltaTime();
	float Time_GetUnscaledDeltaTime();
	void Time_SetTimeScale(float scale);
	float Time_GetTimeScale();


	/*!***********************************************************************
		\brief
			Gets the mouse position
		\return
			glm::vec3 of the mouse position
	*************************************************************************/
	glm::vec3 Input_GetMousePosition();

	/*!***********************************************************************
		\brief
			Gets the mouse scroll delta
		\return
			glm::vec3 of the mouse scroll delta
	*************************************************************************/
	glm::vec3 Input_GetMouseScrollDelta();


	/*!***********************************************************************
		\brief
			Checks if a key is pressed
		\param[in] key
			Key to check
		\return
			True if pressed, false if not
	*************************************************************************/
	bool Input_GetKey(int key);

	/*!***********************************************************************
		\brief
			Checks if a key is triggered
		\param[in] key
			Key to check
		\return
			True if triggered, false if not
	*************************************************************************/
	bool Input_GetKeyDown(int key);

	/*!***********************************************************************
		\brief
			Checks if a key is released
		\param[in] key
			Key to check
		\return
			True if released, false if not
	*************************************************************************/
	bool Input_GetKeyUp(int key);

	/*!***********************************************************************
		\brief
			Checks if a mouse is pressed
		\param[in] key
			Key to check
		\return
			True if pressed, false if not
	*************************************************************************/
	bool Input_GetMouse(int key);

	/*!***********************************************************************
		\brief
			Checks if a mouse is triggered
		\param[in] key
			Key to check
		\return
			True if triggered, false if not
	*************************************************************************/
	bool Input_GetMouseDown(int key);

	/*!***********************************************************************
		\brief
			Checks if a mouse is released
		\param[in] key
			Key to check
		\return
			True if released, false if not
	*************************************************************************/
	bool Input_GetMouseUp(int key);

	float Input_GetAxis(MonoString* axis);

	float Input_GetAxisRaw(MonoString* axis);

	void Cursor_GetVisibility(bool* outVisibility);
	void Cursor_SetVisibility(bool* visibility);

	/*!***********************************************************************
		\brief
			Get translation of translation component
		\param[in] uuid
			UUID of the entity
		\param[in] outTranslation
			Pointer to the translation
	*************************************************************************/
	void TransformComponent_GetTranslation(UUID uuid, glm::vec3* outTranslation);
	void TransformComponent_GetLocalTranslation(UUID uuid, glm::vec3* outTranslation);

	/*!***********************************************************************
		\brief
			Set translation of translation component
		\param[in] uuid
			UUID of the entity
		\param[out] translation
			Pointer to the translation
	*************************************************************************/
	void TransformComponent_SetTranslation(UUID uuid, glm::vec3* translation);
	void TransformComponent_SetLocalTranslation(UUID uuid, glm::vec3* translation);

	/*!***********************************************************************
		\brief
			Get rotation of rotation component
		\param[in] uuid
			UUID of the entity
		\param[in] outRotation
			Pointer to the rotation
	*************************************************************************/
	void TransformComponent_GetRotation(UUID uuid, glm::vec3* outRotation);
	void TransformComponent_GetLocalRotation(UUID uuid, glm::vec3* outRotation);

	/*!***********************************************************************
		\brief
			Set rotation of rotation component
		\param[in] uuid
			UUID of the entity
		\param[out] rotation
			Pointer to the rotation
	*************************************************************************/
	void TransformComponent_SetRotation(UUID uuid, glm::vec3* rotation);
	void TransformComponent_SetLocalRotation(UUID uuid, glm::vec3* rotation);

	/*!***********************************************************************
		\brief
			Get scale of scale component
		\param[in] uuid
			UUID of the entity
		\param[in] outScale
			Pointer to the scale
	*************************************************************************/
	void TransformComponent_GetScale(UUID uuid, glm::vec3* outScale);
	void TransformComponent_GetLocalScale(UUID uuid, glm::vec3* outScale);

	/*!***********************************************************************
		\brief
			Set scale of scale component
		\param[in] uuid
			UUID of the entity
		\param[out] scale
			Pointer to the scale
	*************************************************************************/
	void TransformComponent_SetScale(UUID uuid, glm::vec3* scale);
	void TransformComponent_SetLocalScale(UUID uuid, glm::vec3* scale);

	void TransformComponent_GetParentID(UUID uuid, UUID* parentID);
	void TransformComponent_SetParentID(UUID uuid, UUID* parentID);

	void TransformComponent_GetChildCount(UUID uuid, int* count);
	void TransformComponent_GetChild(UUID uuid, int index, UUID* count);

	void TextComponent_GetText(UUID uuid, MonoString** text);
	void TextComponent_SetText(UUID uuid, MonoString* text);
	void TextComponent_GetColor(UUID uuid, glm::vec4* color);
	void TextComponent_SetColor(UUID uuid, glm::vec4* color);

	void RigidbodyComponent_AddForce(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_AddImpulse(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_AddTorque(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_Move(UUID uuid, glm::vec3* vec);

	void RigidbodyComponent_GetLinearVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetLinearVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetAngularVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetAngularVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetPosition(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetPosition(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetRotation(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetRotation(UUID uuid, glm::vec3* velocity);

	void RigidbodyComponent_SetIsKinematic(UUID uuid, bool* kinematic);
	void RigidbodyComponent_GetIsKinematic(UUID uuid, bool* kinematic);


	/*!***********************************************************************
		\brief
			Get color of sprite renderer component
		\param[in] uuid
			UUID of the entity
		\param[in] outColor
			Pointer to the color
	*************************************************************************/
	void SpriteRendererComponent_GetColor(UUID uuid, glm::vec4* outColor);

	/*!***********************************************************************
		\brief
			Set color of sprite renderer component
		\param[in] uuid
			UUID of the entity
		\param[out] color
			Pointer to the color
	*************************************************************************/
	void SpriteRendererComponent_SetColor(UUID uuid, glm::vec4* color);

	void SpriteRendererComponent_GetSprite(UUID uuid, UUID* spriteID);
	void SpriteRendererComponent_SetSprite(UUID uuid, UUID* spriteID);

	void MeshRendererComponent_GetMaterial(UUID uuid, UUID* materialID);
	void MeshRendererComponent_SetMaterial(UUID uuid, UUID* materialID);

	void MeshRendererComponent_GetEnabled(UUID uuid, bool* state);
	void MeshRendererComponent_SetEnabled(UUID uuid, bool* state);

	void OutlineComponent_GetEnabled(UUID uuid, bool* state);
	void OutlineComponent_SetEnabled(UUID uuid, bool* state);

	void ColliderComponent_GetBounds(UUID uuid, glm::vec3* center, glm::vec3* extents, glm::vec3* min, glm::vec3* max, glm::vec3* size);
	void ColliderComponent_UpdateScale(UUID uuid);

	void AnimatorComponent_SetCurrentAnimation(UUID uuid, UUID animation);
	void AnimatorComponent_GetCurrentAnimation(UUID uuid, UUID* animation);
	void AnimatorComponent_SetBlend(UUID uuid, float blendValue);
	void AnimatorComponent_GetBlend(UUID uuid, float* blendValue);
	void AnimatorComponent_SetSpeed(UUID uuid, float blendValue);
	void AnimatorComponent_GetSpeed(UUID uuid, float* blendValue);
	void AnimatorComponent_SetLooping(UUID uuid, bool blendValue);
	void AnimatorComponent_GetLooping(UUID uuid, bool* blendValue);
	void AnimatorComponent_SetNextAnimation(UUID uuid, UUID animation);
	void AnimatorComponent_GetNextAnimation(UUID uuid, UUID* animation);
	void AnimatorComponent_SwapBlendBuffer(UUID uuid);
	void AnimatorComponent_GetCurrentTime(UUID uuid, float* currentTime);
	void AnimatorComponent_GetAnimationDuration(UUID uuid, float* duration);


	void SceneManager_SetMainCamera(uint64_t entityID);
	void SceneManager_SetActiveScene(MonoString* sceneName);
	void SceneManager_Quit();
	

	void Material_GetSprite(UUID uuid, UUID* spriteID);
	void Material_SetSprite(UUID uuid, UUID* spriteID);


	/*!***********************************************************************
		\brief
			Adds a monobehaviour to Script Component
		\param[in] entityID
			Entity ID to add the component from
		\param[in] reflectionType
			Type of the component to add
	*************************************************************************/
	void ScriptComponent_AddComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	/*!***********************************************************************
		\brief
			Removes a monobehaviour from Script Component
		\param[in] entityID
			Entity ID to remove the component from
		\param[in] reflectionType
			Type of the component to remove
	*************************************************************************/
	void ScriptComponent_RemoveComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	/*!***********************************************************************
		\brief
			Checks if a monobehaviour is in Script Component
		\param[in] entityID
			Entity ID to check the component from
		\param[in] reflectionType
			Type of the component to check
		\return
			True if has, false if not
	*************************************************************************/
	bool ScriptComponent_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType);

	void LayerMask_LayerToName(int layer, MonoString* name);

	void LayerMask_NameToLayer(MonoString* name, int* layer);

	void LayerMask_GetMask(int* layer, MonoArray* stringArray);

	bool Physics_Raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance, int layerMask,
			uint64_t* entityID, float* distance, glm::vec3* normal, glm::vec3* point);

	void Physics_RaycastAll(glm::vec3 origin, glm::vec3 direction, float maxDistance, int layerMask,
			MonoArray** entityIDArray, MonoArray** distanceArray, MonoArray** normalArray, MonoArray** pointArray);

	void CharacterController_Move(uint64_t id, glm::vec3* motion);
	void CharacterController_Jump(uint64_t id, float jumpSpeed);

	void CharacterController_IsGrounded(uint64_t id, bool* grounded);
	void CharacterController_GetLinearVelocity(uint64_t id, glm::vec3* vel);
	void CharacterController_SetLinearVelocity(uint64_t id, glm::vec3* vel);

	void AudioSource_GetClip(uint64_t ID, uint64_t* ClipID);
	void AudioSource_SetClip(uint64_t ID, uint64_t* ClipID);
	void AudioSource_PlayOneShot(uint64_t ID, float volume, uint64_t ClipID);
	void AudioSource_Play(uint64_t ID);
	void AudioSource_IsPlaying(uint64_t ID,  bool* playing);
	void AudioSource_GetLooping(uint64_t ID,  bool* looping);
	void AudioSource_SetLooping(uint64_t ID,  bool* looping);
	void AudioSource_GetVolume(uint64_t ID,  float* volume);
	void AudioSource_SetVolume(uint64_t ID,  float* volume);

}

#endif