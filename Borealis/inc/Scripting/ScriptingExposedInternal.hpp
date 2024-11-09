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
		static std::unordered_map<MonoType*, HasComponentFn> mHasComponentFunctions;
		static std::unordered_map<MonoType*, AddComponentFn> mAddComponentFunctions;
		static std::unordered_map<MonoType*, RemoveComponentFn> mRemoveComponentFunctions;
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

	float Time_GetDeltaTime();

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

	/*!***********************************************************************
		\brief
			Set translation of translation component
		\param[in] uuid
			UUID of the entity
		\param[out] translation
			Pointer to the translation
	*************************************************************************/
	void TransformComponent_SetTranslation(UUID uuid, glm::vec3* translation);

	/*!***********************************************************************
		\brief
			Get rotation of rotation component
		\param[in] uuid
			UUID of the entity
		\param[in] outRotation
			Pointer to the rotation
	*************************************************************************/
	void TransformComponent_GetRotation(UUID uuid, glm::vec3* outRotation);

	/*!***********************************************************************
		\brief
			Set rotation of rotation component
		\param[in] uuid
			UUID of the entity
		\param[out] rotation
			Pointer to the rotation
	*************************************************************************/
	void TransformComponent_SetRotation(UUID uuid, glm::vec3* rotation);

	/*!***********************************************************************
		\brief
			Get scale of scale component
		\param[in] uuid
			UUID of the entity
		\param[in] outScale
			Pointer to the scale
	*************************************************************************/
	void TransformComponent_GetScale(UUID uuid, glm::vec3* outScale);

	/*!***********************************************************************
		\brief
			Set scale of scale component
		\param[in] uuid
			UUID of the entity
		\param[out] scale
			Pointer to the scale
	*************************************************************************/
	void TransformComponent_SetScale(UUID uuid, glm::vec3* scale);

	void RigidbodyComponent_AddForce(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_AddImpulse(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_AddTorque(UUID uuid, glm::vec3* force);

	void RigidbodyComponent_GetLinearVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetLinearVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetAngularVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetAngularVelocity(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetPosition(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetPosition(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_GetRotation(UUID uuid, glm::vec3* velocity);
	void RigidbodyComponent_SetRotation(UUID uuid, glm::vec3* velocity);

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



}

#endif