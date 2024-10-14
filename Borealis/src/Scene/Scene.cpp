/******************************************************************************/
/*!
\file		Scene.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Implements the class for Scene

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <glm/glm.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Entity.hpp>
#include <Scene/ScriptEntity.hpp>
#include <Scene/Components.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Graphics/Renderer3D.hpp>
#include <Core/LoggerSystem.hpp>
#include "Audio/AudioEngine.hpp"
#include <Scene/SceneCamera.hpp>
#include "Graphics/Light.hpp"
#include <Physics/PhysicsSystem.hpp>

namespace Borealis
{
	Scene::Scene(std::string name, std::string path) : mName(name), mScenePath(path)
	{

	}

	Scene::~Scene()
	{
		auto view = mRegistry.view<RigidBodyComponent>();
		for (auto entity : view)
		{
			PhysicsSystem::FreeRigidBody(view.get<RigidBodyComponent>(entity));
		}
	}
	void Scene::UpdateRuntime(float dt)
	{
		if (hasRuntimeStarted)
		{
			{
				mRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& component)
					{
						if (!component.Instance)
						{
							component.Instance = component.Init();
							component.Instance->mEntity = Entity{ entity, this };
							component.Instance->Start();
						}
						component.Instance->Update(dt);
					});
			}

			// Update for scripts -> make it more effecient by doing event-based and
			// overridden-based rather than running every script every loop.
			auto view = mRegistry.view<ScriptComponent>();
			for (auto entity : view)
			{
				auto& scriptComponent = view.get<ScriptComponent>(entity);
				for (auto& [name, script] : scriptComponent.mScripts)
				{
					script->Update();
				}
			}
			static float accumDt = 0;
			accumDt += dt;
			int timeStep = std::max(1, (int)(accumDt / 1.66667f));
			accumDt -= timeStep * 1.66667f;

			auto BTview = mRegistry.view<BehaviourTreeComponent>();
			for (auto entity : BTview)
			{
				BTview.get<BehaviourTreeComponent>(entity).Update(dt);
			}



			//timeStep = dt / 1.66667f;
			for (auto entity : view)
			{
				auto& scriptComponent = view.get<ScriptComponent>(entity);
				for (auto& [name, script] : scriptComponent.mScripts)
				{
					for (int i = 0; i < timeStep; i++)
						script->FixedUpdate();
				}
			}

			//------------------------
			// Physics Simulation here
			//------------------------
			
			// Set Jolt values to entity transform.
			{
				auto physicsGroup = mRegistry.group<>(entt::get<TransformComponent, RigidBodyComponent>);
				for (auto entity : physicsGroup)
				{
					auto [transform, rigidbody] = physicsGroup.get<TransformComponent, RigidBodyComponent>(entity);
					PhysicsSystem::PushTransform(rigidbody.bodyID, transform);
				}

				PhysicsSystem::Update(dt);

				// Set entity values to Jolt transform.
				for (auto entity : physicsGroup)
				{
					auto [transform, rigidbody] = physicsGroup.get<TransformComponent, RigidBodyComponent>(entity);
					PhysicsSystem::PullTransform(rigidbody.bodyID, transform);
				}
				for (auto entity : view)
				{
					auto& scriptComponent = view.get<ScriptComponent>(entity);
					for (auto& [name, script] : scriptComponent.mScripts)
					{
						script->LateUpdate();
					}
				}
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4 mainCameratransform(1.f);
	
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, CameraComponent>);
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					//camera.Camera.SetCameraType(SceneCamera::CameraType::Perspective);
					mainCamera = &camera.Camera;
					mainCameratransform = transform;
					break;
				}
			}
		}

		// Pre-Render
		if (mainCamera)
		{
			Renderer3D::Begin(*mainCamera, mainCameratransform);
			{
				{
					auto group = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);
					for (auto& entity : group)
					{
						auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
						lightComponent.offset = transform.Translate;
						Renderer3D::AddLight(lightComponent);
					}
				}

				auto group = mRegistry.group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
				for (auto& entity : group)
				{
					auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);
					auto groupLight = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);

					Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, (int)entity);
				}
			}
			Renderer3D::End();

			Renderer2D::Begin(*mainCamera, mainCameratransform);
			{
				auto group = mRegistry.group<>(entt::get<TransformComponent, SpriteRendererComponent>);
				for (auto& entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform, sprite, (int)entity);
				}
				//Renderer2D::End();
			}

			{
				//Renderer2D::Begin(*mainCamera, mainCameratransform);
				auto group = mRegistry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
				for (auto& entity : group)
				{
					auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform, circle.Colour, circle.thickness, circle.fade);
				}
				//Renderer2D::End();
			}

			{
				//Renderer2D::Begin(*mainCamera, mainCameratransform);
				auto group = mRegistry.group<>(entt::get<TransformComponent, TextComponent>);
				for (auto& entity : group)
				{

					auto [transform, text] = group.get<TransformComponent, TextComponent>(entity);
					// multiply text scale into transform
					Renderer2D::DrawString(text.text, text.font, transform, (int)entity);
				}
			}
				Renderer2D::End();
			
		}

		//Audio
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, AudioListenerComponent>);
			int listener = 0;
			for (auto& entity : group)
			{
				auto [transform, audioListener] = group.get<TransformComponent, AudioListenerComponent>(entity);
				if (listener == 0)
				{
					listener = 1;
				}
				if (listener > 1)
				{
					BOREALIS_CORE_ASSERT(false, "More than 1 listener");
				}
			}

			if (listener == 1)
			{
				auto group = mRegistry.group<>(entt::get<TransformComponent, AudioSourceComponent>);
				for (auto& entity : group)
				{
					auto [transform, audio] = group.get<TransformComponent, AudioSourceComponent>(entity);
					if (audio.isPlaying && (!Borealis::AudioEngine::isSoundPlaying(audio.channelID) || !audio.isLoop))
					{
						AudioEngine::StopChannel(audio.channelID);
						audio.isPlaying = false;
						audio.channelID = Borealis::AudioEngine::PlayAudio(audio, {}, audio.Volume, audio.isMute, audio.isLoop);
						//audio.channelID = Borealis::AudioEngine::PlayAudio(audio.audio->AudioPath, {}, audio.Volume, audio.isMute, audio.isLoop);
					}
				}
			}
		}
	}

	void Scene::UpdateEditor(float dt, EditorCamera& camera)
	{
		Renderer3D::Begin(camera);
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				lightComponent.offset = transform.Translate;
				Renderer3D::AddLight(lightComponent);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);
				auto groupLight = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);

				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, (int)entity);
			}
		}

		Renderer2D::Begin(camera);
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, SpriteRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform, circle.Colour, circle.thickness, circle.fade, (int)entity);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, TextComponent>);
			for (auto& entity : group)
			{
				auto [transform, text] = group.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.text, text.font, transform, (int)entity);
			}
		}

		Renderer2D::End();

	}
	Entity Scene::CreateEntity(const std::string& name)
	{

		UUID id;
		static unsigned unnamedID = 1;
		auto entity = Entity(mRegistry.create(), this);
		entity.AddComponent<IDComponent>(id);
		name == "" ? entity.AddComponent<TagComponent>("unnamedEntity" + std::to_string(unnamedID++)) : entity.AddComponent<TagComponent>(name);
		entity.AddComponent<TransformComponent>();
		mEntityMap[id] = entity;
		return entity;
	}
	Entity Scene::CreateEntityWithUUID(const std::string& name, uint64_t UUID)
	{
		static unsigned unnamedID = 1;
		auto entity = Entity(mRegistry.create(), this);
		entity.AddComponent<IDComponent>(UUID);
		name == "" ? entity.AddComponent<TagComponent>("unnamedEntity" + std::to_string(unnamedID++)) : entity.AddComponent<TagComponent>(name);
		entity.AddComponent<TransformComponent>();
		mEntityMap[UUID] = entity;
		return entity;
	}
	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (mEntityMap.find(uuid) != mEntityMap.end())
			return { mEntityMap.at(uuid), this };

		return {};
	}
	void Scene::DestroyEntity(Entity entity)
	{		
		mEntityMap.erase(entity.GetUUID());
		if (entity.HasComponent<RigidBodyComponent>())
		{
			PhysicsSystem::FreeRigidBody(entity.GetComponent<RigidBodyComponent>());
		}
		mRegistry.destroy(entity);
	}

	template <typename Component>
	static void CopyComponent(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	template<>
	static void CopyComponent<RigidBodyComponent>(Entity dst, Entity src)
	{
		if (src.HasComponent<RigidBodyComponent>())
			dst.AddOrReplaceComponent<RigidBodyComponent>(src.GetComponent<RigidBodyComponent>());

		if ((bool)dst.GetComponent<RigidBodyComponent>().isBox)
		{
			PhysicsSystem::UpdateBoxValues(dst.GetComponent<RigidBodyComponent>());
		}
		else
		{
			PhysicsSystem::UpdateSphereValues(dst.GetComponent<RigidBodyComponent>());
		}
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		name+= " (clone)";
		Entity newEntity = CreateEntity(name);
		CopyComponent<TransformComponent>(newEntity, entity);
		CopyComponent<SpriteRendererComponent>(newEntity, entity);
		CopyComponent<CameraComponent>(newEntity, entity);
		CopyComponent<NativeScriptComponent>(newEntity, entity);
		CopyComponent<MeshFilterComponent>(newEntity,entity);
		CopyComponent<MeshRendererComponent>(newEntity,entity);
		CopyComponent<BoxColliderComponent>(newEntity,entity);
		CopyComponent<CapsuleColliderComponent>(newEntity,entity);
		CopyComponent<RigidBodyComponent>(newEntity, entity);
		CopyComponent<LightComponent>(newEntity, entity);
		CopyComponent<CircleRendererComponent>(newEntity, entity);
		CopyComponent<TextComponent>(newEntity, entity);
		CopyComponent<AudioSourceComponent>(newEntity, entity);
		CopyComponent<AudioListenerComponent>(newEntity, entity);
		CopyComponent<ScriptComponent>(newEntity, entity);
		CopyComponent<BehaviourTreeComponent>(newEntity, entity);
	}

	void Scene::ResizeViewport(const uint32_t& width, const uint32_t& height)
	{
		mViewportWidth = width;
		mViewportHeight = height;

		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	template <typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID,entt::entity>& entitymap)
	{
		auto view = src.view<Component>();
		for (auto srcEntity : view)
		{
			UUID uuid = src.get<IDComponent>(srcEntity).ID;
			auto dstEntity = entitymap.at(uuid);

			auto srcComponent = view.get<Component>(srcEntity);
			dst.emplace_or_replace<Component>(dstEntity, srcComponent);
		}
	}

	template <>
	static void CopyComponent <ScriptComponent> (entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entitymap)
	{
		auto view = src.view<ScriptComponent>();
		for (auto srcEntity : view)
		{
			UUID uuid = src.get<IDComponent>(srcEntity).ID;
			auto dstEntity = entitymap.at(uuid);

			auto srcComponent = view.get<ScriptComponent>(srcEntity);

			auto& newScriptComponent = dst.emplace<ScriptComponent>(dstEntity);


			for (auto script : srcComponent.mScripts)
			{
				Ref<ScriptInstance> newScript = MakeRef<ScriptInstance>(script.second->GetScriptClass());
				newScript->Init(uuid);
				newScriptComponent.AddScript(script.first, newScript);
			}
		}
	}

	template <>
	static void CopyComponent <RigidBodyComponent>(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entitymap)
	{
		auto view = src.view<RigidBodyComponent>();
		for (auto srcEntity : view)
		{
			UUID uuid = src.get<IDComponent>(srcEntity).ID;
			auto dstEntity = entitymap.at(uuid);

			auto rbComponent = view.get<RigidBodyComponent>(srcEntity);

			auto& newRbComponent = dst.emplace<RigidBodyComponent>(dstEntity);

			newRbComponent = rbComponent;
			if (newRbComponent.isBox == RigidBodyType::Box)
			{
				PhysicsSystem::addSquareBody(newRbComponent.radius, dst.get<TransformComponent>(dstEntity).Translate, newRbComponent);
			}
			else
			{
				PhysicsSystem::addSphereBody(newRbComponent.radius, dst.get<TransformComponent>(dstEntity).Translate, newRbComponent);
			}
		}
	}

	Ref<Scene> Scene::Copy(const Ref<Scene>& other)
	{
		Ref<Scene> newScene = MakeRef<Scene>();
		newScene->mViewportWidth = other->mViewportWidth;
		newScene->mViewportHeight = other->mViewportHeight;

		newScene->mScenePath = other->mScenePath;
		newScene->mName = other->mName;

		std::unordered_map<UUID, entt::entity> UUIDtoENTT;

		auto& originalRegistry = other->mRegistry;
		auto& newRegistry = newScene->mRegistry;
		auto idView = originalRegistry.view<IDComponent>();
		for (auto entity : idView)
		{
			UUID uuid = originalRegistry.get<IDComponent>(entity).ID;
			const auto& name = originalRegistry.get<TagComponent>(entity).Tag;
			UUIDtoENTT[uuid] = newScene->CreateEntityWithUUID(name, uuid);
		}

		CopyComponent<TransformComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<SpriteRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CameraComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<NativeScriptComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<MeshFilterComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<MeshRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<BoxColliderComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CapsuleColliderComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<RigidBodyComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<LightComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CircleRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<TextComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<AudioSourceComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<AudioListenerComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<ScriptComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<BehaviourTreeComponent>(newRegistry, originalRegistry, UUIDtoENTT);

		return newScene;
	}

	void Scene::RuntimeStart()
	{
		hasRuntimeStarted = true;
	}

	void Scene::RuntimeEnd()
	{
		hasRuntimeStarted = false;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.Primary)
			{
				return Entity{ entity, this };
			}
		}
		return Entity{};
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(mViewportWidth, mViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MeshFilterComponent>(Entity entity, MeshFilterComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CapsuleColliderComponent>(Entity entity, CapsuleColliderComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		PhysicsSystem::addSphereBody(component.radius, transform.Translate, component);
	}
	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<AudioListenerComponent>(Entity entity, AudioListenerComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<BehaviourTreeComponent>(Entity entity, BehaviourTreeComponent& component)
	{

	}
}
