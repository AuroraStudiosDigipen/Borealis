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
#include <Scripting/ScriptingUtils.hpp>
#include <Scene/SceneManager.hpp>
#include <Core/UUID.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/InputSystem.hpp>
#include <Core/ApplicationManager.hpp>
#include <Core/TimeManager.hpp>
#include <Core/LayerList.hpp>
#include <mono/metadata/appdomain.h>
#include <Physics/PhysicsSystem.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Assets/AssetManager.hpp>
#include <Audio/AudioEngine.hpp>
namespace Borealis
{
	std::unordered_map<std::string, HasComponentFn> GCFM::mHasComponentFunctions;
	std::unordered_map<std::string, AddComponentFn> GCFM::mAddComponentFunctions;
	std::unordered_map<std::string, RemoveComponentFn> GCFM::mRemoveComponentFunctions;
#define BOREALIS_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Borealis.InternalCalls::" #Name, Name)
	void RegisterInternals()
	{
		//BOREALIS_ADD_INTERNAL_CALL(GetComponent<TransformComponent>);
		BOREALIS_ADD_INTERNAL_CALL(GenerateUUID);
		BOREALIS_ADD_INTERNAL_CALL(Log);
		BOREALIS_ADD_INTERNAL_CALL(LogError);
		BOREALIS_ADD_INTERNAL_CALL(LogWarning);
		BOREALIS_ADD_INTERNAL_CALL(DrawLine);
		BOREALIS_ADD_INTERNAL_CALL(CreateEntity);
		BOREALIS_ADD_INTERNAL_CALL(SetActive);

		BOREALIS_ADD_INTERNAL_CALL(Entity_AddComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_HasComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_SetActive);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetActive);
		BOREALIS_ADD_INTERNAL_CALL(Entity_FindEntity);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetComponent);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetName);
		BOREALIS_ADD_INTERNAL_CALL(Entity_SetName);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetTag);
		BOREALIS_ADD_INTERNAL_CALL(Entity_SetTag);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetEntitiesFromTag);
		BOREALIS_ADD_INTERNAL_CALL(Entity_GetEntitiesFromLayer);


		BOREALIS_ADD_INTERNAL_CALL(Time_GetDeltaTime);
		BOREALIS_ADD_INTERNAL_CALL(Time_GetUnscaledDeltaTime);
		BOREALIS_ADD_INTERNAL_CALL(Time_SetTimeScale);
		BOREALIS_ADD_INTERNAL_CALL(Time_GetTimeScale);

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
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetLocalTranslation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetLocalTranslation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetLocalRotation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetLocalRotation);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetLocalScale);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetLocalScale);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetParentID);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_SetParentID);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetChildCount);
		BOREALIS_ADD_INTERNAL_CALL(TransformComponent_GetChild);

		BOREALIS_ADD_INTERNAL_CALL(TextComponent_GetText);
		BOREALIS_ADD_INTERNAL_CALL(TextComponent_SetText);
		BOREALIS_ADD_INTERNAL_CALL(TextComponent_GetColor);
		BOREALIS_ADD_INTERNAL_CALL(TextComponent_SetColor);

		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetSprite);
		BOREALIS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetSprite);

		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_GetMaterial);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_SetMaterial);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_GetEnabled);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_SetEnabled);

		BOREALIS_ADD_INTERNAL_CALL(OutlineComponent_GetEnabled);
		BOREALIS_ADD_INTERNAL_CALL(OutlineComponent_SetEnabled);

		BOREALIS_ADD_INTERNAL_CALL(ColliderComponent_GetBounds);
		BOREALIS_ADD_INTERNAL_CALL(ColliderComponent_UpdateScale);
		BOREALIS_ADD_INTERNAL_CALL(Material_GetSprite);

		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddForce);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddTorque);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_AddImpulse);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_Move);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_GetLinearVelocity);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_SetLinearVelocity);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_GetAngularVelocity);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_SetAngularVelocity);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_GetPosition);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_SetPosition);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_GetRotation);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_SetRotation);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_SetIsKinematic);
		BOREALIS_ADD_INTERNAL_CALL(RigidbodyComponent_GetIsKinematic);

		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_AddComponent);
		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_RemoveComponent);
		BOREALIS_ADD_INTERNAL_CALL(ScriptComponent_HasComponent);

		BOREALIS_ADD_INTERNAL_CALL(LayerMask_GetMask);
		BOREALIS_ADD_INTERNAL_CALL(LayerMask_LayerToName);
		BOREALIS_ADD_INTERNAL_CALL(LayerMask_NameToLayer);

		BOREALIS_ADD_INTERNAL_CALL(Physics_Raycast);
		BOREALIS_ADD_INTERNAL_CALL(Physics_RaycastAll);

		BOREALIS_ADD_INTERNAL_CALL(CharacterController_Move);
		BOREALIS_ADD_INTERNAL_CALL(CharacterController_Jump);
		BOREALIS_ADD_INTERNAL_CALL(CharacterController_IsGrounded);
		BOREALIS_ADD_INTERNAL_CALL(CharacterController_GetLinearVelocity);
		BOREALIS_ADD_INTERNAL_CALL(CharacterController_SetLinearVelocity);


		BOREALIS_ADD_INTERNAL_CALL(AudioSource_GetClip);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_SetClip);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_PlayOneShot);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_Play);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_IsPlaying );
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_GetLooping);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_SetLooping);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_GetVolume );
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_SetVolume );

		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SetNextAnimation);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetNextAnimation);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SetCurrentAnimation);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetCurrentAnimation);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SetSpeed);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetSpeed);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SetLooping);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetLooping);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SetBlend);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetBlend);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_SwapBlendBuffer);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetCurrentTime);
		BOREALIS_ADD_INTERNAL_CALL(AnimatorComponent_GetAnimationDuration);


		BOREALIS_ADD_INTERNAL_CALL(SceneManager_SetActiveScene);
		BOREALIS_ADD_INTERNAL_CALL(SceneManager_Quit);
		BOREALIS_ADD_INTERNAL_CALL(SceneManager_SetMainCamera);
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

	void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
	{
		Renderer2D::DrawLine(start, end, color);
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

	float Time_GetUnscaledDeltaTime()
	{
		return TimeManager::GetUnscaledDeltaTime();
	}

	void Time_SetTimeScale(float scale)
	{
		TimeManager::SetTimeScale(scale);
	}

	float Time_GetTimeScale()
	{
		return TimeManager::GetTimeScale();
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
		char* typeName = mono_type_get_name(CPPType);
		std::string strName(typeName);
		mono_free(typeName);
		if (GCFM::mRemoveComponentFunctions.find(strName) != GCFM::mRemoveComponentFunctions.end())
		{
			GCFM::mRemoveComponentFunctions.at(strName)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.RemoveComponent");
			
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
		char* typeName = mono_type_get_name(CPPType);
		std::string strName(typeName);
		mono_free(typeName);
		if (GCFM::mAddComponentFunctions.find(typeName) != GCFM::mAddComponentFunctions.end())
		{
			GCFM::mAddComponentFunctions.at(typeName)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.AddComponent");
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
		char* typeName = mono_type_get_name(CPPType);
		std::string strName(typeName);
		mono_free(typeName);

		if (GCFM::mHasComponentFunctions.find(strName) != GCFM::mHasComponentFunctions.end())
		{
			return GCFM::mHasComponentFunctions.at(strName)(Entity);
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to create component: Entity.HasComponent");
			return false;
		}
	}
	void Entity_GetComponent(uint64_t entityID, MonoReflectionType* reflectionType, MonoObject** component)
	{
		MonoType* monoType = mono_reflection_type_get_type(reflectionType);
		char* typeName = mono_type_get_name(monoType);
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& scriptComponent = entity.GetComponent<ScriptComponent>();
		// Filter out to the last dot
		std::string type = typeName;
		type = type.substr(type.find_last_of('.') + 1);


		auto& script = scriptComponent.mScripts[type];
		*component = script->GetInstance();
	}

	//gameObject.SetActive(false)
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
	void Entity_GetName(uint64_t entityID, MonoString** name)
	{
		if (entityID == 0)
		{
			return;
		}
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		std::string str = entity.GetComponent<TagComponent>().Name;
		*name = mono_string_new(mono_domain_get(), str.c_str());
	}
	void Entity_SetName(uint64_t entityID, MonoString* name)
	{
		if (entityID == 0)
		{
			return;
		}
		char* message = mono_string_to_utf8(name);
		std::string str = message;
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TagComponent>().Name = str;
	}
	void Entity_GetTag(uint64_t entityID, MonoString** tag)
	{
		if (entityID == 0)
		{
			return;
		}
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		std::string str = entity.GetComponent<TagComponent>().Tag;
		*tag = mono_string_new(mono_domain_get(), str.c_str());
	}
	void Entity_SetTag(uint64_t entityID, MonoString* tag)
	{
		if (entityID == 0)
		{
			return;
		}
		char* message = mono_string_to_utf8(tag);
		std::string str = message;
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TagComponent>().Tag = str;
		TagList::AddEntity(str, entityID);
	}
	void Entity_FindEntity(MonoString* name, UUID* ID)
	{
		char* message = mono_string_to_utf8(name);
		std::string entityName = message;
		auto entityList = SceneManager::GetActiveScene()->GetRegistry().view<TagComponent>();
		for (auto entity : entityList)
		{
			auto tag = entityList.get<TagComponent>(entity);
			if (tag.Name == entityName)
			{
				*ID = SceneManager::GetActiveScene()->GetRegistry().get<IDComponent>(entity).ID;
				return;
			}
		}
		*ID = 0;
	}
	void Entity_GetEntitiesFromLayer(int32_t layerNum, MonoArray** objectArray)
	{
		std::unordered_set<UUID> entities;
		bool output = LayerList::getEntitiesAtLayer(layerNum, &entities);
		*objectArray = mono_array_new(mono_domain_get(), mono_get_object_class(), entities.size());
		int counter = 0;
		for (auto id : entities)
		{
			MonoObject* obj;
			InitGameObject(obj, id, "GameObject");
			mono_array_set(*objectArray, MonoObject*, counter++, obj);
		}
	}
	void Entity_GetEntitiesFromTag(MonoString* tag, MonoArray** objectArray)
	{
		char* message = mono_string_to_utf8(tag);
		std::string entityTag = message;
		auto List = TagList::getEntitiesAtTag(entityTag);

		*objectArray = mono_array_new(mono_domain_get(), mono_get_object_class(), List.size());
		int counter = 0;
		for (auto id : List)
		{
			MonoObject* obj;
			InitGameObject(obj, id, "GameObject");
			mono_array_set(*objectArray, MonoObject*, counter++, obj);
		}
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
		auto& transform = entity.GetComponent<TransformComponent>();
		*outTranslation = transform.GetGlobalTranslate();
	}
	void TransformComponent_SetTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& tc = entity.GetComponent<TransformComponent>();
		auto rotation = tc.GetGlobalRotation();
		auto scale = tc.GetGlobalScale();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), *translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		tc.SetGlobalTransform(transform);
	}
	void TransformComponent_GetRotation(UUID uuid, glm::vec3* outRotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& transform = entity.GetComponent<TransformComponent>();
		*outRotation = transform.GetGlobalRotation();
	
	}
	void TransformComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& tc = entity.GetComponent<TransformComponent>();
		auto translate = tc.GetGlobalTranslate();
		auto scale = tc.GetGlobalScale();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translate) * glm::toMat4(glm::quat(glm::radians(*rotation))) * glm::scale(glm::mat4(1.0f), scale);
		tc.SetGlobalTransform(transform);

	}
	void TransformComponent_GetScale(UUID uuid, glm::vec3* outScale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& transform = entity.GetComponent<TransformComponent>();
		*outScale = transform.GetGlobalScale();
	}
	void TransformComponent_SetScale(UUID uuid, glm::vec3* scale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& tc = entity.GetComponent<TransformComponent>();

		auto translate = tc.GetGlobalTranslate();
		auto rotation = tc.GetGlobalRotation();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translate) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), *scale);
		tc.SetGlobalTransform(transform);
	}

	void TransformComponent_GetLocalTranslation(UUID uuid, glm::vec3* outTranslation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outTranslation = entity.GetComponent<TransformComponent>().Translate;
	}
	void TransformComponent_SetLocalTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().Translate = *translation;
	}
	void TransformComponent_GetLocalRotation(UUID uuid, glm::vec3* outRotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outRotation = entity.GetComponent<TransformComponent>().Rotation;

	}
	void TransformComponent_SetLocalRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}
	void TransformComponent_GetLocalScale(UUID uuid, glm::vec3* outScale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}
	void TransformComponent_SetLocalScale(UUID uuid, glm::vec3* scale)
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
		*parentID = entity.GetComponent<TransformComponent>().ParentID;
	}
	void TransformComponent_SetParentID(UUID uuid, UUID* parentID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		Entity parent = scene->GetEntityByUUID(*parentID);
		auto& transform = entity.GetComponent<TransformComponent>();
		transform.ResetParent(entity);
		if (*parentID != 0)
			transform.SetParent(entity, parent);
	}
	void TransformComponent_GetChildCount(UUID uuid, int* count)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*count = (int)entity.GetComponent<TransformComponent>().ChildrenID.size();
	}
	void TransformComponent_GetChild(UUID uuid, int index, UUID* count)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (index < entity.GetComponent<TransformComponent>().ChildrenID.size())
		{
			auto it = entity.GetComponent<TransformComponent>().ChildrenID.begin();
			// Advance the iterator 4 steps to reach the 5th element (index 4)
			std::advance(it, index);
			*count = *it;
		}
		else
		{
			*count = 0;
		}
	}
	void TextComponent_GetText(UUID uuid, MonoString** text)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		std::string str = entity.GetComponent<TextComponent>().text;
		*text = mono_string_new(mono_domain_get(), str.c_str());
	}
	void TextComponent_SetText(UUID uuid, MonoString* text)
	{
		char* message = mono_string_to_utf8(text);
		std::string str = message;
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		entity.GetComponent<TextComponent>().text = str;
	}
	void TextComponent_GetColor(UUID uuid, glm::vec4* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*color = entity.GetComponent<TextComponent>().colour;
	}
	void TextComponent_SetColor(UUID uuid, glm::vec4* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<TextComponent>().colour = *color;
	}
	void RigidbodyComponent_AddForce(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		//PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);

	}
	void RigidbodyComponent_AddImpulse(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		//PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);
	}
	void RigidbodyComponent_AddTorque(UUID uuid, glm::vec3* force)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		//PhysicsSystem::AddForce(entity.GetComponent<RigidBodyComponent>().bodyID, *force);
	}
	void RigidbodyComponent_Move(UUID uuid, glm::vec3* vec)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		//PhysicsSystem::move(rb, *vec);
	}
	void RigidbodyComponent_GetLinearVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		*velocity = PhysicsSystem::GetLinearVelocity(entity.GetComponent<BoxColliderComponent>().bodyID);
	}
	void RigidbodyComponent_SetLinearVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		PhysicsSystem::SetLinearVelocity(entity.GetComponent<BoxColliderComponent>().bodyID, *velocity);
	}
	void RigidbodyComponent_GetAngularVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		*velocity = PhysicsSystem::GetAngularVelocity(entity.GetComponent<BoxColliderComponent>().bodyID);
	}
	void RigidbodyComponent_SetAngularVelocity(UUID uuid, glm::vec3* velocity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		PhysicsSystem::SetAngularVelocity(entity.GetComponent<BoxColliderComponent>().bodyID, *velocity);
	}
	void RigidbodyComponent_GetPosition(UUID uuid, glm::vec3* position)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		*position = PhysicsSystem::GetPosition(entity.GetComponent<BoxColliderComponent>().bodyID);
	}
	void RigidbodyComponent_SetPosition(UUID uuid, glm::vec3* position)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		//auto& rb = entity.GetComponent<RigidbodyComponent>();
		PhysicsSystem::SetPosition(entity.GetComponent<BoxColliderComponent>().bodyID, *position);
	}
	void RigidbodyComponent_GetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		//auto& rb = entity.GetComponent<RigidbodyComponent>();
		*rotation = PhysicsSystem::GetRotation(entity.GetComponent<BoxColliderComponent>().bodyID);
	}
	void RigidbodyComponent_SetRotation(UUID uuid, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		PhysicsSystem::SetRotation(entity.GetComponent<BoxColliderComponent>().bodyID, *rotation);
	}
	void RigidbodyComponent_SetIsKinematic(UUID uuid, bool* kinematic)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();
		*kinematic == true? rb.movement = MovementType::Dynamic : rb.movement = MovementType::Static;
	}
	void RigidbodyComponent_GetIsKinematic(UUID uuid, bool* kinematic)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& rb = entity.GetComponent<RigidbodyComponent>();

		if (rb.movement == MovementType::Static)
		{
			*kinematic = false;
		}
		else
		{
			*kinematic = true;
		}
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
	void SpriteRendererComponent_GetSprite(UUID uuid, UUID* spriteID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.GetComponent<SpriteRendererComponent>().Texture)
			*spriteID = entity.GetComponent<SpriteRendererComponent>().Texture->mAssetHandle;
		else
			*spriteID = 0;
	}
	void SpriteRendererComponent_SetSprite(UUID uuid, UUID* spriteID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (*spriteID != 0)
			entity.GetComponent<SpriteRendererComponent>().Texture = AssetManager::GetAsset<Texture2D>(*spriteID);
		else
		{
			entity.GetComponent<SpriteRendererComponent>().Texture = Ref<Texture2D>();
		}
	}
	void MeshRendererComponent_GetMaterial(UUID uuid, UUID* materialID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");

		if (entity.GetComponent<MeshRendererComponent>().Material)
			*materialID = entity.GetComponent<MeshRendererComponent>().Material->mAssetHandle;
		else
			*materialID = 0;
	}
	void MeshRendererComponent_SetMaterial(UUID uuid, UUID* materialID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (*materialID != 0)
			entity.GetComponent<MeshRendererComponent>().Material = AssetManager::GetAsset<Material>(*materialID);
		else
		{
			entity.GetComponent<MeshRendererComponent>().Material = Ref<Material>();
		}
	}
	void MeshRendererComponent_GetEnabled(UUID uuid, bool* state)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*state = entity.GetComponent<MeshRendererComponent>().active;
	}
	void MeshRendererComponent_SetEnabled(UUID uuid, bool* state)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*state = entity.GetComponent<MeshRendererComponent>().active;
	}
	void OutlineComponent_GetEnabled(UUID uuid, bool* state)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*state = entity.GetComponent<OutLineComponent>().active;
	}
	void OutlineComponent_SetEnabled(UUID uuid, bool* state)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<OutLineComponent>().active = *state;
	}
	void ColliderComponent_GetBounds(UUID uuid, glm::vec3* center, glm::vec3* extents, glm::vec3* min, glm::vec3* max, glm::vec3* size)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			auto& collider = entity.GetComponent<CapsuleColliderComponent>();
			*center = collider.center;
			//*extents = collider.Extents;
			//*min = collider.Min;
			//*max = collider.Max;
			//*size = collider.Size;
		}
		else if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& collider = entity.GetComponent<BoxColliderComponent>();
			*center = collider.center;
			//*extents = collider.Extents;
			//*min = collider.Min;
			//*max = collider.Max;
			*size = collider.size;
		}
		else if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& collider = entity.GetComponent<SphereColliderComponent>();
			*center = collider.center;
			/*	*extents = collider.Extents;
				*min = collider.Min;
				*max = collider.Max;
				*size = collider.size;*/
		}
	}
	void ColliderComponent_UpdateScale(UUID uuid)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			auto& collider = entity.GetComponent<CapsuleColliderComponent>();
			PhysicsSystem::UpdateScale(entity.GetComponent<CapsuleColliderComponent>(), entity.GetComponent<TransformComponent>());
		}
		else if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& collider = entity.GetComponent<BoxColliderComponent>();
			PhysicsSystem::UpdateScale(entity.GetComponent<BoxColliderComponent>(), entity.GetComponent<TransformComponent>());
		}
		else if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& collider = entity.GetComponent<SphereColliderComponent>();
			PhysicsSystem::UpdateScale(entity.GetComponent<SphereColliderComponent>(), entity.GetComponent<TransformComponent>());
		}
	}
	void AnimatorComponent_SetCurrentAnimation(UUID uuid, UUID animation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (animation == 0)
		{
			entity.GetComponent<AnimatorComponent>().animator.mNextAnimation = Ref<Animation>();
			return;
		}
		entity.GetComponent<AnimatorComponent>().animation = AssetManager::GetAsset<Animation>(animation);
	}
	void AnimatorComponent_GetCurrentAnimation(UUID uuid, UUID* animation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*animation = entity.GetComponent<AnimatorComponent>().animation->mAssetHandle;
	}
	void AnimatorComponent_SetBlend(UUID uuid, float blendValue)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<AnimatorComponent>().animator.mBlendFactor = blendValue;
	}
	void AnimatorComponent_GetBlend(UUID uuid, float* blendValue)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*blendValue = entity.GetComponent<AnimatorComponent>().animator.mBlendFactor;
	}

	void AnimatorComponent_SetSpeed(UUID uuid, float speed)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<AnimatorComponent>().speed = speed;
	}
	void AnimatorComponent_GetSpeed(UUID uuid, float* speed)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*speed = entity.GetComponent<AnimatorComponent>().speed;
	}

	void AnimatorComponent_SetLooping(UUID uuid, bool looping)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<AnimatorComponent>().loop = looping;
	}
	void AnimatorComponent_GetLooping(UUID uuid, bool* looping)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*looping = entity.GetComponent<AnimatorComponent>().loop;
	}
	void AnimatorComponent_SetNextAnimation(UUID uuid, UUID animation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (animation == 0)
		{
			entity.GetComponent<AnimatorComponent>().animator.mNextAnimation = Ref<Animation>();
			return;
		}
		entity.GetComponent<AnimatorComponent>().animator.mNextAnimation = AssetManager::GetAsset<Animation>(animation);
	}
	void AnimatorComponent_GetNextAnimation(UUID uuid, UUID* animation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*animation = entity.GetComponent<AnimatorComponent>().animator.mNextAnimation->mAssetHandle;
	}
	void AnimatorComponent_SwapBlendBuffer(UUID uuid)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto firstAnimation = entity.GetComponent<AnimatorComponent>().animation;
		entity.GetComponent<AnimatorComponent>().animation = entity.GetComponent<AnimatorComponent>().animator.mNextAnimation;
		entity.GetComponent<AnimatorComponent>().animator.mNextAnimation = firstAnimation;
	}
	void AnimatorComponent_GetCurrentTime(UUID uuid, float* currentTime)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*currentTime = entity.GetComponent<AnimatorComponent>().animator.GetCurrentAnimationTime();
	}
	void AnimatorComponent_GetAnimationDuration(UUID uuid, float* duration)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*duration = entity.GetComponent<AnimatorComponent>().animator.GetAnimationDuration();
	}
	void SceneManager_SetMainCamera(uint64_t entityID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(entityID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.HasComponent<CameraComponent>())
		{
			auto view = SceneManager::GetActiveScene()->GetRegistry().view<CameraComponent>();
			for (auto entity : view)
			{
				auto& camera = view.get<CameraComponent>(entity);
				camera.Primary = false;
			}
			entity.GetComponent<CameraComponent>().Primary = true;
		}


	}
	void SceneManager_SetActiveScene(MonoString* sceneName)
	{
		std::string sceneNme = mono_string_to_utf8(sceneName);
		SceneManager::NextSceneName = sceneNme;
		SceneManager::ToNextScene = true;
	}
	void SceneManager_Quit()
	{
		ApplicationManager::Get().SetIsRunning(false);
	}
	void Material_GetSprite(UUID uuid, UUID* spriteID)
	{
		*spriteID = AssetManager::GetAsset<Material>(uuid)->GetTextureMaps().at(Material::Albedo)->mAssetHandle;
	}
	void Material_SetSprite(UUID uuid, UUID* spriteID)
	{
		AssetManager::GetAsset<Material>(uuid)->SetTextureMap(Material::Albedo, AssetManager::GetAsset<Texture2D>(*spriteID));
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
	void LayerMask_LayerToName(int layer, MonoString* name)
	{
		if (LayerList::HasIndex(layer))
		{
			std::string layerName = LayerList::IndexToLayer(layer);
			name = mono_string_new(mono_domain_get(), layerName.c_str());
		}
		else
		{
			APP_LOG_WARN("Layer does not exist");
		}
	}
	void LayerMask_NameToLayer(MonoString* name, int* layer)
	{
		std::string layerName = mono_string_to_utf8(name);
		if (LayerList::HasLayer(layerName))
		{
			*layer = LayerList::LayerToIndex(layerName);
		}
		else
		{
			APP_LOG_WARN("Layer does not exist");
			*layer = 0;
		}
	}
	void LayerMask_GetMask(int* layer, MonoArray* stringArray)
	{
		int size = mono_array_length(stringArray);
		std::vector<std::string> layerNames;
		for (int i = 0; i < size; i++)
		{
			MonoString* str = (MonoString*)mono_array_get(stringArray, MonoString*, i);
			char* message = mono_string_to_utf8(str);
			std::string layerName = message;
			layerNames.push_back(layerName);
		}
		*layer = 0;
		for (auto& name : layerNames)
		{
			if (LayerList::HasLayer(name))
			{
				*layer |= 1 << LayerList::LayerToIndex(name);
			}
			else
			{
				APP_LOG_WARN("Layer does not exist");
			}
		}
	}
	bool Physics_Raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance, int layerMask, uint64_t* entityID, float* distance, glm::vec3* normal, glm::vec3* point)
	{
		RaycastHit result;
		bool output = PhysicsSystem::RayCast(origin, direction, &result, maxDistance, layerMask);
		if (output)
		{
			*entityID = result.ID;
			*distance = result.distance;
			*normal = result.normal;
			*point = result.point;
		}
		return output;
	}
	void Physics_RaycastAll(glm::vec3 origin, glm::vec3 direction, float maxDistance, int layerMask, MonoArray** entityIDArray, MonoArray** distanceArray, MonoArray** normalArray, MonoArray** pointArray)
	{
		std::vector<RaycastHit> results = PhysicsSystem::RayCastAll(origin, direction, maxDistance, layerMask);

		*entityIDArray = mono_array_new(mono_domain_get(), mono_get_uint64_class(), results.size());
		*distanceArray = mono_array_new(mono_domain_get(), mono_get_single_class(), results.size());
		*normalArray = mono_array_new(mono_domain_get(), mono_get_single_class(), results.size() * 3);
		*pointArray = mono_array_new(mono_domain_get(), mono_get_single_class(), results.size() * 3);


		float* distance_data = (float*)mono_array_addr_with_size(*distanceArray, sizeof(float), 0);
		uint64_t* ID_data = (uint64_t*)mono_array_addr_with_size(*entityIDArray, sizeof(uint64_t), 0);
		glm::vec3* normal_data = (glm::vec3*)mono_array_addr_with_size(*normalArray, sizeof(glm::vec3), 0);
		glm::vec3* point_data = (glm::vec3*)mono_array_addr_with_size(*pointArray, sizeof(glm::vec3), 0);

		for (int i = 0; i < results.size(); i++)
		{
			ID_data[i] = results[i].ID;
			distance_data[i] = results[i].distance;
			normal_data[i] = results[i].normal;
			point_data[i] = results[i].point;
		}
	}
	void CharacterController_Move(uint64_t id, glm::vec3* motion)
	{
		Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(id);
		if (entity.HasComponent<CharacterControllerComponent>())
		{
			entity.GetComponent<CharacterControllerComponent>().inMovementDirection = *motion;
		}
	
	}
	void CharacterController_Jump(uint64_t id, float jumpSpeed)
	{
		Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(id);
		if (entity.HasComponent<CharacterControllerComponent>())
		{
			entity.GetComponent<CharacterControllerComponent>().jumpSpeed = jumpSpeed;
			entity.GetComponent<CharacterControllerComponent>().isJump = true;
		}
	}
	void CharacterController_IsGrounded(uint64_t id, bool* grounded)
	{
		Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(id);
		if (entity.HasComponent<CharacterControllerComponent>())
		{
			*grounded = PhysicsSystem::IsCharacterOnGround(entity.GetComponent<CharacterControllerComponent>().controller);
		}
		else
		{
			*grounded = false;
		}
	}

	void CharacterController_GetLinearVelocity(uint64_t id, glm::vec3* vel)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(id);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*vel = PhysicsSystem::GetLinearVelocity(entity.GetComponent<CharacterControllerComponent>().controller);
	}

	void CharacterController_SetLinearVelocity(uint64_t id, glm::vec3* vel)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(id);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		PhysicsSystem::SetLinearVelocity(entity.GetComponent<CharacterControllerComponent>().controller, *vel);
	}

	void AudioSource_GetClip(uint64_t ID, uint64_t* ClipID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.GetComponent<AudioSourceComponent>().audio)
		{
			*ClipID = entity.GetComponent<AudioSourceComponent>().audio->mAssetHandle;
		}
		else
		{
			*ClipID = 0;
		}
	}
	void AudioSource_SetClip(uint64_t ID, uint64_t* ClipID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (*ClipID != 0)
		{
			entity.GetComponent<AudioSourceComponent>().audio = AssetManager::GetAsset<Audio>(*ClipID);
		}
		else
		{
			entity.GetComponent<AudioSourceComponent>().audio = Ref<Audio>();
		}
	}
	void AudioSource_PlayOneShot(uint64_t ID, float volume, uint64_t ClipID)
	{
		if (ClipID != 0)
		{
			Ref<Audio> audio = AssetManager::GetAsset<Audio>(ClipID);
			if (audio)
			{
				Scene* scene = SceneManager::GetActiveScene().get();
				BOREALIS_CORE_ASSERT(scene, "Scene is null");
				Entity entity = scene->GetEntityByUUID(ID);
				BOREALIS_CORE_ASSERT(entity, "Entity is null");
				auto& transform = entity.GetComponent<TransformComponent>();
				auto translate = transform.GetGlobalTranslate();
				auto& audioSource = entity.GetComponent<AudioSourceComponent>();
				AudioEngine::PlayOneShot(audio, translate, volume, audioSource.group);
			}
		}
		
	}
	void AudioSource_Play(uint64_t ID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& transform = entity.GetComponent<TransformComponent>();
		auto translate = transform.GetGlobalTranslate();
		auto& audioSource = entity.GetComponent<AudioSourceComponent>();
		if (audioSource.audio)
			/*AudioEngine::PlayAudio(audioSource, translate, audioSource.Volume, audioSource.isMute, audioSource.isLoop)*/
			AudioEngine::Play(audioSource.audio, translate, audioSource.Volume, audioSource.isLoop, "BGM");
	}
	void AudioSource_IsPlaying(uint64_t ID, bool* playing)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*playing = entity.GetComponent<AudioSourceComponent>().isPlaying;
	}
	void AudioSource_GetLooping(uint64_t ID, bool* looping)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*looping = entity.GetComponent<AudioSourceComponent>().isLoop;
	}
	void AudioSource_SetLooping(uint64_t ID, bool* looping)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<AudioSourceComponent>().isLoop = *looping;
	}
	void AudioSource_GetVolume(uint64_t ID, float* volume)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*volume = entity.GetComponent<AudioSourceComponent>().Volume;
	}
	void AudioSource_SetVolume(uint64_t ID, float* volume)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<AudioSourceComponent>().Volume = *volume;
	}
}