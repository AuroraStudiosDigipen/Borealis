/******************************************************************************/
/*!
\file		ScriptingExposedInternal.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 13, 2024
\brief		Defines the functions to expose to C# Scripting Environment

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Scripting/ScriptingExposedInternal.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Scene/SceneManager.hpp>
#include <Core/UUID.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/InputSystem.hpp>
#include <Core/ApplicationManager.hpp>
#include <Core/TimeManager.hpp>


namespace Borealis
{
	std::unordered_map<MonoType*, HasComponentFn> GCFM::mHasComponentFunctions;
	std::unordered_map<MonoType*, AddComponentFn> GCFM::mAddComponentFunctions;
	std::unordered_map<MonoType*, RemoveComponentFn> GCFM::mRemoveComponentFunctions;
#define BOREALIS_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Borealis.InternalCalls::" #Name, Name)
	void RegisterInternals()
	{
		//BOREALIS_ADD_INTERNAL_CALL(GetComponent<TransformComponent>);
		BOREALIS_ADD_INTERNAL_CALL(GenerateUUID);
		BOREALIS_ADD_INTERNAL_CALL(Log);
		BOREALIS_ADD_INTERNAL_CALL(CreateEntity);
		BOREALIS_ADD_INTERNAL_CALL(SetActive);

		BOREALIS_ADD_INTERNAL_CALL(Entity_AddComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_HasComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_SetActive);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetActive);

		BOREALIS_ADD_INTERNAL_CALL(Time_GetDeltaTime);

		BOREALIS_ADD_INTERNAL_CALL(Input_GetMousePosition);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetMouseScrollDelta);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetKey);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetKeyDown);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetKeyUp);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetMouse);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetMouseDown);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetMouseUp);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetAxis);
		BOREALIS_ADD_INTERNAL_CALL(Input_GetAxisRaw);

		BOREALIS_ADD_INTERNAL_CALL(Cursor_GetVisibility);
		BOREALIS_ADD_INTERNAL_CALL(Cursor_SetVisibility);

		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetScale);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetParentID);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetParentID);

		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);

		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddForce);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddTorque);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddImpulse);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_Move);

		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_AddComponent);
		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_RemoveComponent);
		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_HasComponent);
	}
	uint64_t GenerateUUID()
	{
		return UUID();
	}
	void Log(MonoString* text)
	{
		char* message = mono_string_to_utf8(text);
		std::string logMessage = message;
		mono_free(message);
		APP_LOG_INFO(logMessage.c_str());
	}

	void LogError(MonoString* text)
	{
		char* message = mono_string_to_utf8(text);
		std::string logMessage = message;
		mono_free(message);
		APP_LOG_ERROR(logMessage.c_str());
	}

	void LogWarning(MonoString* text)
	{
		char* message = mono_string_to_utf8(text);
		std::string logMessage = message;
		mono_free(message);
		APP_LOG_WARN(logMessage.c_str());
	}

	uint64_t CreateEntity(MonoString* text)
	{
		char* name = mono_string_to_utf8(text);
		std::string strName = name;
		return SceneManager::GetActiveScene()->CreateEntity(strName).GetUUID();
	}

	void SetActive(bool value, uint64_t* outEntityID)
	{
		auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(*outEntityID);
		if (entity)
		{
			entity.GetComponent<TagComponent>().active = value;
		}
	}

	float Time_GetDeltaTime()
	{
		return TimeManager::GetDeltaTime();
	}

	void Entity_RemoveComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		auto Entity = SceneManager::GetEntity(entityID);

		if (!Entity)
		{
			BOREALIS_CORE_WARN("Entity does not exist: Entity.RemoveComponent");
			return;
		}

		if (reflectionType == nullptr)
		{
			BOREALIS_CORE_WARN("Reflection type is null: Entity.RemoveComponent");
			return;
		}

		MonoType* CPPType = mono_reflection_type_get_type(reflectionType);
		if (GCFM::mRemoveComponentFunctions.find(CPPType) != GCFM::mRemoveComponentFunctions.end())
		{
			GCFM::mRemoveComponentFunctions.at(CPPType)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.RemoveComponent");
			char* typeName = mono_type_get_name(CPPType);
			mono_free(typeName);
			return;
		}
	}

	void Entity_AddComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		auto Entity = SceneManager::GetEntity(entityID);

		if (!Entity)
		{
			BOREALIS_CORE_WARN("Entity does not exist: Entity.AddComponent");
			return;
		}

		if (reflectionType == nullptr)
		{
			BOREALIS_CORE_WARN("Reflection type is null: Entity.AddComponent");
			return;
		}

		MonoType* CPPType = mono_reflection_type_get_type(reflectionType);
		if (GCFM::mAddComponentFunctions.find(CPPType) != GCFM::mAddComponentFunctions.end())
		{
			GCFM::mAddComponentFunctions.at(CPPType)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.AddComponent");
			char* typeName = mono_type_get_name(CPPType);
			mono_free(typeName);
			return;
		}
	}

	bool Entity_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		auto Entity = SceneManager::GetEntity(entityID);

		if (!Entity)
		{
			BOREALIS_CORE_WARN("Entity does not exist: Entity.HasComponent");
			return false;
		}

		if (reflectionType == nullptr)
		{
			BOREALIS_CORE_WARN("Reflection type is null: Entity.HasComponent");
			return false;
		}

		MonoType* CPPType = mono_reflection_type_get_type(reflectionType);
		if (GCFM::mHasComponentFunctions.find(CPPType) != GCFM::mHasComponentFunctions.end())
		{
			return GCFM::mHasComponentFunctions.at(CPPType)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.HasComponent");
			char* typeName = mono_type_get_name(CPPType);
			mono_free(typeName);
			return false;
		}
	}
	void Entity_SetActive(uint64_t entityID, bool* active)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TagComponent>().active = *active;
	}
	void Entity_GetActive(uint64_t entityID, bool* active)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*active = entity.GetComponent<TagComponent>().active;
	}
	glm::vec3 Input_GetMousePosition()
	{
		return {InputSystem::GetMouseX(), InputSystem::GetMouseY(), 0.0f};
	}
	glm::vec3 Input_GetMouseScrollDelta()
	{
		return { 0.f, InputSystem::GetScroll() , 0.f};
	}

	bool Input_GetKey(int key)
	{
		return InputSystem::IsKeyPressed(key);
	}
	bool Input_GetKeyDown(int key)
	{
		return InputSystem::IsKeyTriggered(key);
	}
	bool Input_GetKeyUp(int key)
	{
		return InputSystem::IsKeyReleased(key);
	}
	bool Input_GetMouse(int key)
	{
		return InputSystem::IsMouseButtonPressed(key);
	}
	bool Input_GetMouseDown(int key)
	{
		return InputSystem::IsMouseButtonTriggered(key);
	}
	bool Input_GetMouseUp(int key)
	{
		return InputSystem::IsMouseButtonReleased(key);
	}
	float Input_GetAxis(MonoString* axis)
	{
		char* message = mono_string_to_utf8(axis);
		std::string axisString = message;
		
		if (axisString == "Horizontal")
		{
			return InputSystem::GetMouseDeltaX();
		}
		else if (axisString == "Vertical")
		{
			return InputSystem::GetMouseDeltaY();
		}
		return 0;
	}
	float Input_GetAxisRaw(MonoString* axis)
	{
		char* message = mono_string_to_utf8(axis);
		std::string axisString = message;

		if (axisString == "Horizontal")
		{
			return InputSystem::GetMouseDeltaXRaw();
		}
		else if (axisString == "Vertical")
		{
			return InputSystem::GetMouseDeltaYRaw();
		}
		return 0;
	}
	void Cursor_GetVisibility(bool* outVisibility)
	{
		*outVisibility = ApplicationManager::Get().GetWindow()->GetCursorVisibility();
	}
	void Cursor_SetVisibility(bool* visibility)
	{
		ApplicationManager::Get().GetWindow()->SetCursorVisibility(*visibility);
	}
	void TransformComponent_GetTranslation(UUID uuid, glm::vec3* outTranslation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outTranslation = entity.GetComponent<TransformComponent>().Translate;
	}
	void TransformComponent_SetTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().Translate = *translation;
	}
	void TransformComponent_GetRotation(UUID uuid, glm::vec3* outRotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	
	}
	void TransformComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}
	void TransformComponent_GetScale(UUID uuid, glm::vec3* outScale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}
	void TransformComponent_SetScale(UUID uuid, glm::vec3* scale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().Scale = *scale;
	}
	void TransformComponent_GetParentID(UUID uuid, UUID* parentID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().ParentID = *parentID;
	}
	void TransformComponent_SetParentID(UUID uuid, UUID* parentID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		Entity parent = scene->GetEntityByUUID(*parentID);
		TransformComponent::ResetParent(entity);
		if (*parentID != 0)
			TransformComponent::SetParent(entity, parent);
	}
	void RigidbodyComponent_AddForce(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);

	}
	void RigidbodyComponent_AddImpulse(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);
	}
	void RigidbodyComponent_AddTorque(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);
	}
	void RigidbodyComponent_Move(UUID uuid, glm::vec3* vec)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::move(rb, *vec);
	}
	void RigidbodyComponent_GetLinearVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		*velocity = PhysicsSystem::GetLinearVelocity(entity.GetComponent<RigidBodyComponent>().bodyID);
	}
	void RigidbodyComponent_SetLinearVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::SetLinearVelocity(entity.GetComponent<RigidBodyComponent>().bodyID, *velocity);
	}
	void RigidbodyComponent_GetAngularVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		*velocity = PhysicsSystem::GetAngularVelocity(entity.GetComponent<RigidBodyComponent>().bodyID);
	}
	void RigidbodyComponent_SetAngularVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::SetAngularVelocity(entity.GetComponent<RigidBodyComponent>().bodyID, *velocity);
	}
	void RigidbodyComponent_GetPosition(UUID uuid, glm::vec3* position)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		*position = PhysicsSystem::GetPosition(entity.GetComponent<RigidBodyComponent>().bodyID);
	}
	void RigidbodyComponent_SetPosition(UUID uuid, glm::vec3* position)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::SetPosition(entity.GetComponent<RigidBodyComponent>().bodyID, *position);
	}
	void RigidbodyComponent_GetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		*rotation = PhysicsSystem::GetRotation(entity.GetComponent<RigidBodyComponent>().bodyID);
	}
	void RigidbodyComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		PhysicsSystem::SetRotation(entity.GetComponent<RigidBodyComponent>().bodyID, *rotation);
	}
	void SpriteRendererComponent_GetColor(UUID uuid, glm::vec4* outColor)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<SpriteRendererComponent>().Colour = *outColor;
	}
	void SpriteRendererComponent_SetColor(UUID uuid, glm::vec4* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<SpriteRendererComponent>().Colour = *color;
	}
	void ScriptComponent_AddComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		auto Entity = SceneManager::GetEntity(entityID);

		if (!Entity)
		{
			BOREALIS_CORE_WARN("Entity does not exist: ScriptComponent.AddComponent");
			return;
		}

		if (reflectionType == nullptr)
		{
			BOREALIS_CORE_WARN("Reflection type is null: ScriptComponent.AddComponent");
			return;
		}

		MonoClass* klass = mono_class_from_mono_type(mono_reflection_type_get_type(reflectionType));
		std::string className = mono_class_get_name(klass);

		if (ScriptingSystem::mScriptClasses.find(className) == ScriptingSystem::mScriptClasses.end())
		{
			BOREALIS_CORE_WARN("Failed to create component: ScriptComponent.AddComponent"); // Not a mono behaviour
			return;
		}

		Ref<ScriptInstance> instance = MakeRef<ScriptInstance>(ScriptingSystem::mScriptClasses.at(className));
		instance->Init(Entity.GetUUID());
		if (Entity.HasComponent<ScriptComponent>() == false)
		{
			Entity.AddComponent<ScriptComponent>();
		}
		Entity.GetComponent<ScriptComponent>().AddScript(instance->GetKlassName(), instance);
	}
	void ScriptComponent_RemoveComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		if (ScriptComponent_HasComponent(entityID, reflectionType))
		{
			Entity entity = SceneManager::GetEntity(entityID);
			entity.GetComponent<ScriptComponent>().RemoveScript(mono_class_get_name(mono_class_from_mono_type(mono_reflection_type_get_type(reflectionType))));
			if (entity.GetComponent<ScriptComponent>().mScripts.empty())
			{
				entity.RemoveComponent<ScriptComponent>();
			}
		}
		else
		{
			BOREALIS_CORE_WARN("Component does not exist!");
		}
	}
	bool ScriptComponent_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		auto Entity = SceneManager::GetEntity(entityID);

		if (!Entity)
		{
			BOREALIS_CORE_WARN("Entity does not exist: ScriptComponent.AddComponent");
			return false;
		}

		if (reflectionType == nullptr)
		{
			BOREALIS_CORE_WARN("Reflection type is null: ScriptComponent.AddComponent");
			return false;
		}

		if (Entity.HasComponent<ScriptComponent>() == false)
		{
			return false;
		}

		MonoClass* klass = mono_class_from_mono_type(mono_reflection_type_get_type(reflectionType));
		std::string className = mono_class_get_name(klass);

		return Entity.GetComponent<ScriptComponent>().HasScript(className);
	}
}