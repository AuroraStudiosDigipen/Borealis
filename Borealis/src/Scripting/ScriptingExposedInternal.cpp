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
		BOREALIS_ADD_INTERNAL_CALL(SetFullscreen);
		BOREALIS_ADD_INTERNAL_CALL(SetMasterVolume);
		BOREALIS_ADD_INTERNAL_CALL(SetSFXVolume);
		BOREALIS_ADD_INTERNAL_CALL(SetMusicVolume);

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
		BOREALIS_ADD_INTERNAL_CALL(Input_GetMouseViewportPosition);
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

		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_GetColor);
		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_SetColor);
		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_GetThickness);
		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_SetThickness);
		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_GetFade);
		BOREALIS_ADD_INTERNAL_CALL(CircleRendererComponent_SetFade);


		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_GetMaterial);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_SetMaterial);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_GetEnabled);
		BOREALIS_ADD_INTERNAL_CALL(MeshRendererComponent_SetEnabled);

		BOREALIS_ADD_INTERNAL_CALL(OutlineComponent_GetEnabled);
		BOREALIS_ADD_INTERNAL_CALL(OutlineComponent_SetEnabled);

		BOREALIS_ADD_INTERNAL_CALL(ColliderComponent_GetBounds);
		BOREALIS_ADD_INTERNAL_CALL(ColliderComponent_UpdateScale);
		BOREALIS_ADD_INTERNAL_CALL(ColliderComponent_SetActive);
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

		BOREALIS_ADD_INTERNAL_CALL(AudioSource_Stop);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_PlayOneShot);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_PlayOneShotLabel);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_PlayOneShotPosition);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_IsPlaying );
		BOREALIS_ADD_INTERNAL_CALL(AudioListener_SetListener);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_StopID);
		BOREALIS_ADD_INTERNAL_CALL(AudioSource_IsChannelPlaying);

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

		BOREALIS_ADD_INTERNAL_CALL(UIAnimator_SetPlaying);
		BOREALIS_ADD_INTERNAL_CALL(UIAnimator_GetPlaying);
		BOREALIS_ADD_INTERNAL_CALL(UIAnimator_GetEndLoop);

		BOREALIS_ADD_INTERNAL_CALL(SceneManager_SetActiveScene);
		BOREALIS_ADD_INTERNAL_CALL(SceneManager_Quit);
		BOREALIS_ADD_INTERNAL_CALL(SceneManager_SetMainCamera);
		BOREALIS_ADD_INTERNAL_CALL(SceneManager_SetGamma);

		BOREALIS_ADD_INTERNAL_CALL(SetAudioClipName);
		BOREALIS_ADD_INTERNAL_CALL(GetAudioClipName);

		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetDuration);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetDuration);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetLooping);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetLooping);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartDelay);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartDelay);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartLifeTime);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartLifeTime);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartSpeed);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartSpeed);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Get3DStartSizeBool);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Set3DStartSizeBool);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRandomStartSize);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRandomStartSize);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartSize);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartSize);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartSize2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartSize2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Get3DRandomStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Set3DRandomStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRandomStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRandomStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartRotation2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartRotation2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRandomStartColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRandomStartColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetStartColor2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetStartColor2);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetEndColorBool);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetEndColorBool);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetEndColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetEndColor);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetGravityModifier);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetGravityModifier);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetSimulationSpeed);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetSimulationSpeed);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetMaxParticles);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetMaxParticles);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRateOverTime);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRateOverTime);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetAngle);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetAngle);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRadius);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRadius);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRadiusThickness);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRadiusThickness);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetScale);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetScale);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetRotation);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetBillboard);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetBillboard);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetUseNoise);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetUseNoise);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetNoiseStrength);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetNoiseStrength);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetNoiseFrequency);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetNoiseFrequency);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetNoiseScrollSpeed);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetNoiseScrollSpeed); 
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetTexture);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetTexture);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_GetShape);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_SetShape);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Start);
		BOREALIS_ADD_INTERNAL_CALL(ParticleSystem_Stop);
		BOREALIS_ADD_INTERNAL_CALL(Canvas_GetAlpha);
		BOREALIS_ADD_INTERNAL_CALL(Canvas_SetAlpha);
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

		if (value)
		{
			//Activate
			if (entity.HasComponent<BoxColliderComponent>()) {
				if (entity.GetComponent<BoxColliderComponent>().isActive)
					PhysicsSystem::SetActive(entity.GetComponent<BoxColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				if (entity.GetComponent<CapsuleColliderComponent>().isActive)
					PhysicsSystem::SetActive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				if (entity.GetComponent<SphereColliderComponent>().isActive)
					PhysicsSystem::SetActive(entity.GetComponent<SphereColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				if (entity.GetComponent<CylinderColliderComponent>().isActive)
					PhysicsSystem::SetActive(entity.GetComponent<CylinderColliderComponent>().bodyID);
			}
		}

		else

		{
			// decativate
			if (entity.HasComponent<BoxColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<BoxColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<SphereColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CylinderColliderComponent>().bodyID);
			}
		}
	}
	

	void SetFullscreen(bool value)
	{
		ApplicationManager::Get().GetWindow()->SetFullScreen(value);
	}

	void SetMasterVolume(float vol)
	{
		AudioEngine::SetMasterVolume(vol);
	}

	void SetSFXVolume(float vol)
	{
		AudioEngine::SetGroupVolume("SFX", vol);
	}

	void SetMusicVolume(float vol)
	{
		AudioEngine::SetGroupVolume("BGM", vol);
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

		if (*active)
		{
			//Activate
			if (entity.HasComponent<BoxColliderComponent>()) {
				if (entity.GetComponent<BoxColliderComponent>().isActive)
				PhysicsSystem::SetActive(entity.GetComponent<BoxColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				if (entity.GetComponent<CapsuleColliderComponent>().isActive)
				PhysicsSystem::SetActive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				if (entity.GetComponent<SphereColliderComponent>().isActive)
				PhysicsSystem::SetActive(entity.GetComponent<SphereColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				if (entity.GetComponent<CylinderColliderComponent>().isActive)
				PhysicsSystem::SetActive(entity.GetComponent<CylinderColliderComponent>().bodyID);
			}
		}

		else

		{
			// decativate
			if (entity.HasComponent<BoxColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<BoxColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<SphereColliderComponent>().bodyID);
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CylinderColliderComponent>().bodyID);
			}
		}
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
	glm::vec3 Input_GetMouseViewportPosition()
	{
		return { InputSystem::GetMouseViewport().first, InputSystem::GetMouseViewport().second, 0.0f };
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
		*outColor = entity.GetComponent<SpriteRendererComponent>().Colour;
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
	void CircleRendererComponent_GetColor(UUID uuid, glm::vec4* outColor)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*outColor = entity.GetComponent<CircleRendererComponent>().Colour;
	}
	void CircleRendererComponent_SetColor(UUID uuid, glm::vec4* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<CircleRendererComponent>().Colour = *color;
	}
	void CircleRendererComponent_GetThickness(UUID uuid, float* thickness)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*thickness = entity.GetComponent<CircleRendererComponent>().thickness;
	}
	void CircleRendererComponent_SetThickness(UUID uuid, float* thickness)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<CircleRendererComponent>().thickness = *thickness;
	}
	void CircleRendererComponent_GetFade(UUID uuid, float* fade)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*fade = entity.GetComponent<CircleRendererComponent>().fade;
	}
	void CircleRendererComponent_SetFade(UUID uuid, float* fade)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<CircleRendererComponent>().fade = *fade;
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
		else if (entity.HasComponent<CylinderColliderComponent>())
		{
			auto& collider = entity.GetComponent<CylinderColliderComponent>();
			*center = collider.center;
			//*extents = collider.Extents;
			//*min = collider.Min;
			//*max = collider.Max;
			//*size = collider.Size;
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
		else if (entity.HasComponent<CylinderColliderComponent>())
		{
			auto& collider = entity.GetComponent<CylinderColliderComponent>();
			PhysicsSystem::UpdateScale(entity.GetComponent<CylinderColliderComponent>(), entity.GetComponent<TransformComponent>());
		}
	}
	void ColliderComponent_SetActive(UUID uuid, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		if (*value)
		{
			//Activate
			if (entity.HasComponent<BoxColliderComponent>()) {
				PhysicsSystem::SetActive(entity.GetComponent<BoxColliderComponent>().bodyID);
				entity.GetComponent<BoxColliderComponent>().isActive = true;
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				PhysicsSystem::SetActive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
				entity.GetComponent<CapsuleColliderComponent>().isActive = true;
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				PhysicsSystem::SetActive(entity.GetComponent<SphereColliderComponent>().bodyID);
				entity.GetComponent<SphereColliderComponent>().isActive = true;
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				PhysicsSystem::SetActive(entity.GetComponent<CylinderColliderComponent>().bodyID);
				entity.GetComponent<CylinderColliderComponent>().isActive = true;
			}
		}
		else
		{
			// decativate
			if (entity.HasComponent<BoxColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<BoxColliderComponent>().bodyID);
				entity.GetComponent<BoxColliderComponent>().isActive = false;
			}

			if (entity.HasComponent<CapsuleColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CapsuleColliderComponent>().bodyID);
				entity.GetComponent<CapsuleColliderComponent>().isActive = false;
			}

			if (entity.HasComponent<SphereColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<SphereColliderComponent>().bodyID);
				entity.GetComponent<SphereColliderComponent>().isActive = false;
			}

			if (entity.HasComponent<CylinderColliderComponent>()) {
				PhysicsSystem::SetInactive(entity.GetComponent<CylinderColliderComponent>().bodyID);
				entity.GetComponent<CylinderColliderComponent>().isActive = false;
			}
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

	void UIAnimator_SetPlaying(UUID uuid, bool playing)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<UIAnimatorComponent>().animator.mIsPlaying = playing;
	}

	void UIAnimator_GetPlaying(UUID uuid, bool* playing)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*playing = entity.GetComponent<UIAnimatorComponent>().animator.mIsPlaying;
	}

	void UIAnimator_GetEndLoop(UUID uuid, bool* endLoop)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(uuid);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*endLoop = entity.GetComponent<UIAnimatorComponent>().animator.mLoopEnd;
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
	void SceneManager_SetGamma(float gamma)
	{
		SceneManager::GetActiveScene()->GetSceneRenderConfig().ubo.gamma = gamma;
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

	int AudioSource_PlayOneShot(uint64_t ID, MonoArray* arr, MonoArray* values, MonoArray* param)
	{
			Scene* scene = SceneManager::GetActiveScene().get();
			BOREALIS_CORE_ASSERT(scene, "Scene is null");
			Entity entity = scene->GetEntityByUUID(ID);
			BOREALIS_CORE_ASSERT(entity, "Entity is null");
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& audioSource = entity.GetComponent<AudioSourceComponent>();

			std::array<uint8_t, 16>  id;
			for (int i = 0; i < 16; i++)
			{
				id[i] = mono_array_get(arr, uint8_t, i);
			}

			std::unordered_map<std::string, float> floatMap;
			if (param != nullptr)
			for (int i = 0; i <  mono_array_length(param); i++)
			{
				MonoString* monoStr = (MonoString*)mono_array_get(param, MonoObject*, i);
				float val = mono_array_get(values, float, i);
				const char* cStr = mono_string_to_utf8(monoStr);
				floatMap[cStr] = val;
			}

			audioSource.channelID = AudioEngine::PlayOneShot(id, floatMap, transform.GetGlobalTransform());
			return audioSource.channelID;
			
	}

	int AudioSource_PlayOneShotLabel(uint64_t ID, MonoArray* arr, MonoArray* values, MonoArray* param)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& audioSource = entity.GetComponent<AudioSourceComponent>();

		std::array<uint8_t, 16>  id;
		for (int i = 0; i < 16; i++)
		{
			id[i] = mono_array_get(arr, uint8_t, i);
		}

		std::unordered_map<std::string, std::string> stringMap;
		if (param != nullptr)
		for (int i = 0; i < mono_array_length(param); i++)
		{
			MonoString* monoStr = (MonoString*)mono_array_get(param, MonoObject*, i);
			MonoString* valStr = (MonoString*)mono_array_get(values, MonoObject*, i);
			const char* cStr = mono_string_to_utf8(monoStr);
			const char* val = mono_string_to_utf8(valStr);
			stringMap[cStr] = val;
		}

		audioSource.channelID = AudioEngine::PlayOneShot(id, stringMap, transform.GetGlobalTransform());
		return audioSource.channelID;

	}

	int AudioSource_PlayOneShotPosition(uint64_t ID, MonoArray* arr, glm::vec3* pos)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& audioSource = entity.GetComponent<AudioSourceComponent>();

		std::array<uint8_t, 16>  id;
		for (int i = 0; i < 16; i++)
		{
			id[i] = mono_array_get(arr, uint8_t, i);
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), *pos);

		audioSource.channelID = AudioEngine::PlayOneShot(id, std::unordered_map<std::string, float>(), transform);
		return audioSource.channelID;
	}

	void AudioSource_Stop(uint64_t ID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		auto& transform = entity.GetComponent<TransformComponent>();
		auto translate = transform.GetGlobalTranslate();
		if (entity.HasComponent<AudioSourceComponent>())
		{
			auto& audioSource = entity.GetComponent<AudioSourceComponent>();
			AudioEngine::StopChannel(audioSource.channelID);
		}
	}
	void AudioSource_StopID(uint64_t Uid, int ID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.HasComponent<AudioSourceComponent>())
		{
			AudioEngine::StopChannel(ID);
		}
	}
	void AudioSource_IsPlaying(uint64_t ID, bool* playing)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.HasComponent<AudioSourceComponent>())
		*playing = AudioEngine::isSoundPlaying(entity.GetComponent<AudioSourceComponent>().channelID);
	}

	void AudioListener_SetListener(uint64_t ID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");

		auto group = scene->GetRegistry().group<>(entt::get<TransformComponent, AudioListenerComponent>);
		for (auto& entity : group)
		{
			Entity brEntity{ entity, scene };
			auto [transform, audioListener] = group.get<TransformComponent, AudioListenerComponent>(entity);

			audioListener.isAudioListener = false;
		}

		entity.GetComponent<AudioListenerComponent>().isAudioListener = true;
	}
	bool AudioSource_IsChannelPlaying(uint64_t uiD, int ID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(ID);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.HasComponent<AudioSourceComponent>())
			return AudioEngine::isSoundPlaying(ID);
		return false;
	}
	MonoString* GetAudioClipName(MonoArray** guid)
	{
		std::array<uint8_t, 16>  id;
		for (int i = 0; i < 16; i++)
		{
			id[i] = mono_array_get(*guid, uint8_t, i);
		}

		auto str = AudioEngine::GetEventNameFromGUID(id);
		return mono_string_new(mono_domain_get(), str.c_str());
	}
	void SetAudioClipName(MonoString* name, MonoArray** guid)
	{
		// get the name
		char* message = mono_string_to_utf8(name);
		std::string audioName = message;
		std::array<uint8_t, 16>  id = AudioEngine::GetGUIDFromEventName(audioName);
		*guid = mono_array_new(mono_domain_get(), mono_get_object_class(), 16); // 16 bytes
		for (int i = 0; i < 16; i++)
		{
			mono_array_set(*guid, uint8_t, i, id[i]);
		}

	}
	void ParticleSystem_GetDuration(uint64_t v, float* duration)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*duration = entity.GetComponent<ParticleSystemComponent>().duration;
	}
	void ParticleSystem_SetDuration(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().duration = *value;
	}
	void ParticleSystem_GetLooping(uint64_t v, bool* looping)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*looping = entity.GetComponent<ParticleSystemComponent>().looping;
	}
	void ParticleSystem_SetLooping(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().looping = *value;
	}
	void ParticleSystem_GetStartDelay(uint64_t v, float* delay)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*delay = entity.GetComponent<ParticleSystemComponent>().startDelay;
	}
	void ParticleSystem_SetStartDelay(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startDelay = *value;
	}
	void ParticleSystem_GetStartLifeTime(uint64_t v, float* lifetime)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*lifetime = entity.GetComponent<ParticleSystemComponent>().startLifeTime;
	}
	void ParticleSystem_SetStartLifeTime(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startLifeTime = *value;
	}
	void ParticleSystem_GetStartSpeed(uint64_t v, float* speed)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*speed = entity.GetComponent<ParticleSystemComponent>().startSpeed;
	}
	void ParticleSystem_SetStartSpeed(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startSpeed = *value;
	}
	void ParticleSystem_Get3DStartSizeBool(uint64_t v, bool* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>()._3DStartSizeBool;
	}
	void ParticleSystem_Set3DStartSizeBool(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>()._3DStartSizeBool = *value;
	}
	void ParticleSystem_GetRandomStartSize(uint64_t v, bool* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>().randomStartSize;
	}
	void ParticleSystem_SetRandomStartSize(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().randomStartSize = *value;
	}
	void ParticleSystem_GetStartSize(uint64_t v, glm::vec3* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>().startSize;
	}
	void ParticleSystem_SetStartSize(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startSize = *value;
	}
	void ParticleSystem_GetStartSize2(uint64_t v, glm::vec3* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>().startSize2;
	}
	void ParticleSystem_SetStartSize2(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startSize2 = *value;
	}
	void ParticleSystem_Get3DRandomStartRotation(uint64_t v, bool* startRotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startRotation = entity.GetComponent<ParticleSystemComponent>()._3DStartRotationBool;
	}
	void ParticleSystem_Set3DRandomStartRotation(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>()._3DStartRotationBool = *value;
	}
	void ParticleSystem_GetRandomStartRotation(uint64_t v, bool* startRotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startRotation = entity.GetComponent<ParticleSystemComponent>().randomStartRotation;
	}
	void ParticleSystem_SetRandomStartRotation(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().randomStartRotation = *value;
	}
	void ParticleSystem_GetStartRotation(uint64_t v, glm::vec3* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>().startRotation;
	}
	void ParticleSystem_SetStartRotation(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startRotation = *value;
	}
	void ParticleSystem_GetStartRotation2(uint64_t v, glm::vec3* startSize)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startSize = entity.GetComponent<ParticleSystemComponent>().startRotation2;
	}
	void ParticleSystem_SetStartRotation2(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startRotation2 = *value;
	}
	void ParticleSystem_GetRandomStartColor(uint64_t v, bool* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*color = entity.GetComponent<ParticleSystemComponent>().randomStartColor;
	}
	void ParticleSystem_SetRandomStartColor(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().randomStartColor = *value;
	}
	void ParticleSystem_GetStartColor(uint64_t v, glm::vec4* startColor2)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startColor2 = entity.GetComponent<ParticleSystemComponent>().startColor;
	}
	void ParticleSystem_SetStartColor(uint64_t v, glm::vec4* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startColor = *value;
	}
	void ParticleSystem_GetStartColor2(uint64_t v, glm::vec4* startColor2)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startColor2 = entity.GetComponent<ParticleSystemComponent>().startColor2;
	}
	void ParticleSystem_SetStartColor2(uint64_t v, glm::vec4* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().startColor2 = *value;
	}
	void ParticleSystem_GetEndColorBool(uint64_t v, bool* color)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*color = entity.GetComponent<ParticleSystemComponent>().endColorBool;
	}
	void ParticleSystem_SetEndColorBool(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().endColorBool = *value;
	}
	void ParticleSystem_GetEndColor(uint64_t v, glm::vec4* startColor2)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*startColor2 = entity.GetComponent<ParticleSystemComponent>().endColor;
	}
	void ParticleSystem_SetEndColor(uint64_t v, glm::vec4* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().endColor = *value;
	}
	void ParticleSystem_GetGravityModifier(uint64_t v, float* gravity)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*gravity = entity.GetComponent<ParticleSystemComponent>().gravityModifer;
	}
	void ParticleSystem_SetGravityModifier(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().gravityModifer = *value;
	}
	void ParticleSystem_GetSimulationSpeed(uint64_t v, float* speed)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*speed = entity.GetComponent<ParticleSystemComponent>().simulationSpeed;
	}
	void ParticleSystem_SetSimulationSpeed(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().simulationSpeed = *value;
	}
	void ParticleSystem_GetMaxParticles(uint64_t v, unsigned* max)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*max = entity.GetComponent<ParticleSystemComponent>().maxParticles;
	}
	void ParticleSystem_SetMaxParticles(uint64_t v, unsigned* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().maxParticles = *value;
	}
	void ParticleSystem_GetRateOverTime(uint64_t v, float* speed)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*speed = entity.GetComponent<ParticleSystemComponent>().rateOverTime;
	}
	void ParticleSystem_SetRateOverTime(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().rateOverTime = *value;
	}
	void ParticleSystem_GetAngle(uint64_t v, float* angle)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*angle = entity.GetComponent<ParticleSystemComponent>().angle;
	}
	void ParticleSystem_SetAngle(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().angle = *value;
	}
	void ParticleSystem_GetRadius(uint64_t v, float* radius)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*radius = entity.GetComponent<ParticleSystemComponent>().radius;
	}
	void ParticleSystem_SetRadius(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().radius = *value;
	}
	void ParticleSystem_GetRadiusThickness(uint64_t v, float* thickness)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*thickness = entity.GetComponent<ParticleSystemComponent>().radiusThickness;
	}
	void ParticleSystem_SetRadiusThickness(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().radiusThickness = *value;
	}
	void ParticleSystem_GetScale(uint64_t v, glm::vec3* scale)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*scale = entity.GetComponent<ParticleSystemComponent>().scale;
	}
	void ParticleSystem_SetScale(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().scale = *value;
	}
	void ParticleSystem_GetRotation(uint64_t v, glm::vec3* rotation)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*rotation = entity.GetComponent<ParticleSystemComponent>().rotation;
	}
	void ParticleSystem_SetRotation(uint64_t v, glm::vec3* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().rotation = *value;
	}
	void ParticleSystem_GetBillboard(uint64_t v, bool* bill)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*bill = entity.GetComponent<ParticleSystemComponent>().billboard;
	}
	void ParticleSystem_SetBillboard(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().billboard = *value;
	}
	void ParticleSystem_GetUseNoise(uint64_t v, bool* noise)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*noise = entity.GetComponent<ParticleSystemComponent>().useNoise;
	}
	void ParticleSystem_SetUseNoise(uint64_t v, bool* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().useNoise = *value;
	}
	void ParticleSystem_GetNoiseStrength(uint64_t v, float* noise)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*noise = entity.GetComponent<ParticleSystemComponent>().noiseStrength;
	}
	void ParticleSystem_SetNoiseStrength(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().noiseStrength = *value;
	}
	void ParticleSystem_GetNoiseFrequency(uint64_t v, float* noise)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*noise = entity.GetComponent<ParticleSystemComponent>().noiseFrequency;
	}
	void ParticleSystem_SetNoiseFrequency(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().noiseFrequency = *value;
	}
	void ParticleSystem_GetNoiseScrollSpeed(uint64_t v, float* noise)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*noise = entity.GetComponent<ParticleSystemComponent>().noiseScrollSpeed;
	}
	void ParticleSystem_SetNoiseScrollSpeed(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().noiseScrollSpeed = *value;
	}
	void ParticleSystem_GetTexture(uint64_t v, uint64_t* textureID)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		if (entity.GetComponent<ParticleSystemComponent>().texture)
		{
			*textureID = entity.GetComponent<ParticleSystemComponent>().texture->mAssetHandle;
		}
		else
		{
			*textureID = 0;
		}
	}

	void ParticleSystem_SetTexture(uint64_t v1, uint64_t v2)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v1);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().texture = AssetManager::GetAsset<Texture2D>(v2);
	}

	void ParticleSystem_GetShape(uint64_t v, int* shape)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*shape = (int)entity.GetComponent<ParticleSystemComponent>().emitterShape;
	}

	void ParticleSystem_SetShape(uint64_t v, int* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().emitterShape = (EmitterShape)*value;
	}

	void ParticleSystem_Start(uint64_t v)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().Start();
	}

	void ParticleSystem_Stop(uint64_t v)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<ParticleSystemComponent>().Stop();
	}

	void Canvas_GetAlpha(uint64_t v, float* alphaValue)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		*alphaValue = entity.GetComponent<CanvasComponent>().alpha;
	}
	void Canvas_SetAlpha(uint64_t v, float* value)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		BOREALIS_CORE_ASSERT(scene, "Scene is null");
		Entity entity = scene->GetEntityByUUID(v);
		BOREALIS_CORE_ASSERT(entity, "Entity is null");
		entity.GetComponent<CanvasComponent>().alpha = *value;
	}
} // End of namespace Borealis

