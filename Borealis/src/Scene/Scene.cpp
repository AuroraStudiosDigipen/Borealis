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
#include <Scripting/ScriptingUtils.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Graphics/Renderer3D.hpp>
#include <Core/LoggerSystem.hpp>
#include "Audio/AudioEngine.hpp"
#include <Scene/SceneCamera.hpp>
#include "Graphics/Light.hpp"
#include <Physics/PhysicsSystem.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <Core/LayerList.hpp>

namespace Borealis
{
	Scene::Scene(std::string name, std::string path) : mName(name), mScenePath(path)
	{
		//FrameBufferProperties props{ 1280, 720, false };
		//props.Attachments = { FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
		//mViewportFrameBuffer = FrameBuffer::Create(props);

		//FrameBufferProperties propsRuntime{ 1280, 720, false };
		//propsRuntime.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger,FramebufferTextureFormat::Depth };
		//mRuntimeFrameBuffer = FrameBuffer::Create(propsRuntime);

		//FrameBufferProperties propsGBuffer{ 1280, 720, false };
		//propsGBuffer.Attachments = 
		//{
		//	FramebufferTextureFormat::RGBA16F,  // Albedo + Alpha
		//	FramebufferTextureFormat::RedInteger,  // entity id
		//	FramebufferTextureFormat::RGBA8,   // Normal + roughness
		//	FramebufferTextureFormat::RGBA8,   // Specular + metallic
		//	//FramebufferTextureFormat::RGB16F,   // Position
		//	FramebufferTextureFormat::Depth     // Depth buffer
		//};
		//mGFrameBuffer = FrameBuffer::Create(propsGBuffer);
	}

	Scene::~Scene()
	{
		auto view = mRegistry.view<RigidBodyComponent>();
	}

	void Scene::Render2DPass()
	{
		//2D pass
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, SpriteRendererComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetGlobalTransform(), sprite, (int)entity);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetGlobalTransform(), circle.Colour, circle.thickness, circle.fade, (int)entity);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, TextComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, text] = group.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.text, text.font, transform, (int)entity);
			}
		}
	}

	void Scene::Render3DPass()
	{
		//3D pass
		{
			entt::basic_group group = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				glm::vec3 buffer = transform.GetGlobalTranslate();
				if (buffer != lightComponent.position)
				{
					lightComponent.position = buffer;
					lightComponent.isEdited = true;

					std::cout << "graph Light x {}" << lightComponent.position.x << '\n';
				}

				buffer = transform.GetGlobalRotation();
				if (buffer != lightComponent.direction)
				{
					lightComponent.direction = buffer;
					lightComponent.isEdited = true;
				}
				Renderer3D::AddLight(lightComponent);
			}
		}
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);
				auto groupLight = mRegistry.group<>(entt::get<TransformComponent, LightComponent>);

				Renderer3D::DrawMesh(transform.GetGlobalTransform(), meshFilter, meshRenderer, (int)entity);
			}
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
							if (component.Instance->mEntity.IsActive())
								component.Instance->Start();
						}
						Entity brEntity{ entity, this };
						if (brEntity.IsActive())
							component.Instance->Update(dt);
					});
			}

			// Update for scripts -> make it more effecient by doing event-based and
			// overridden-based rather than running every script every loop.
			auto view = mRegistry.view<ScriptComponent>();
			for (auto entity : view)
			{
				Entity brEntity{ entity, this };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto& scriptComponent = view.get<ScriptComponent>(entity);
				for (auto& [name, script] : scriptComponent.mScripts)
				{
					if (script->IsActive())
					{
						script->Update();
					}
				}
			}
			static float accumDt = 0.0f; // Accumulated delta time
			const float fixedTimeStep = 1.f/60; // Fixed update interval (~60 FPS)

			accumDt += dt; // Accumulate elapsed time

			// Calculate how many steps to process
			int timeStep = static_cast<int>(accumDt / fixedTimeStep);

			if (timeStep > 0) {
				accumDt -= timeStep * fixedTimeStep; // Reduce accumulated time
			}

			auto BTview = mRegistry.view<BehaviourTreeComponent>();
			for (auto entity : BTview)
			{
				Entity brEntity{ entity, this };
				if (!brEntity.IsActive())
				{
					continue;
				}
				BTview.get<BehaviourTreeComponent>(entity).mBehaviourTrees->Update(dt, brEntity);
			}



			//timeStep = dt / 1.66667f;
			for (auto entity : view)
			{
				Entity brEntity{ entity, this };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto& scriptComponent = view.get<ScriptComponent>(entity);
				for (auto& [name, script] : scriptComponent.mScripts)
				{
					if (script->IsActive())
					{
						for (int i = 0; i < timeStep; i++)
							script->FixedUpdate();
					}
				}
			}

			//------------------------
			// Physics Simulation here
			//------------------------
			
			// thread the push operation maybe next time. TODO
			{
				/*unsigned numThread = std::thread::hardware_concurrency();
				size_t maxSize = physicsGroup.size();
				size_t chunkSize = (maxSize + numThread - 1) / numThread;
				std::vector<std::thread> threads;

				for (unsigned i = 0; i < numThread; i++)
				{
					size_t start = i * chunkSize;
					size_t end = (i + 1) * chunkSize;
					if (end > maxSize)
						end = maxSize;
					threads.emplace_back([start, end, &physicsGroup, this]()
						{
							for (auto entity : physicsGroup)
							{
								Entity brEntity{ entity, this };
								if (!brEntity.IsActive())
								{
									continue;
								}
								auto [transform, rigidbody] = physicsGroup.get<TransformComponent, RigidBodyComponent>(entity);
								PhysicsSystem::PushTransform(rigidbody, transform, brEntity);
							}
						});
				}

				for (auto& thread : threads)
				{
					thread.join();
				}*/

				auto characterGroup = mRegistry.group<>(entt::get<TransformComponent, CharacterControllerComponent>);
				for (auto entity : characterGroup)
				{
					Entity brEntity{ entity, this };
					if (!brEntity.IsActive())
					{
						continue;
					}
					auto [transform, character] = characterGroup.get<TransformComponent, CharacterControllerComponent>(entity);
					PhysicsSystem::PushCharacterTransform(character, transform.Translate, transform.Rotation);
					PhysicsSystem::HandleInput(dt, character);
				}


				for (auto entity : characterGroup)
				{
					Entity brEntity{ entity, this };
					if (!brEntity.IsActive())
					{
						continue;
					}
					auto [transform, character] = characterGroup.get<TransformComponent, CharacterControllerComponent>(entity);
					PhysicsSystem::PrePhysicsUpdate(dt, character.controller);
				}

				for (auto entity : characterGroup)
				{
					Entity brEntity{ entity, this };
					if (!brEntity.IsActive())
					{
						continue;
					}
					auto [transform, character] = characterGroup.get<TransformComponent, CharacterControllerComponent>(entity);
					PhysicsSystem::PullCharacterTransform(character, transform.Translate, transform.Rotation);
				}

				auto boxGroup = mRegistry.group<>(entt::get<TransformComponent, BoxColliderComponent, RigidBodyComponent>);
				
				for (int i = 0; i < timeStep; i++)
				{
					for (auto entity : boxGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, box, rigidbody] = boxGroup.get<TransformComponent, BoxColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PushTransform(box, transform, box.rigidBody);
					}

					auto sphereGroup = mRegistry.group<>(entt::get<TransformComponent, SphereColliderComponent, RigidBodyComponent>);
					for (auto entity : sphereGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, sphere, rigidbody] = sphereGroup.get<TransformComponent, SphereColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PushTransform(sphere, transform, sphere.rigidBody);
					}

					auto capsuleGroup = mRegistry.group<>(entt::get<TransformComponent, CapsuleColliderComponent, RigidBodyComponent>);
					for (auto entity : capsuleGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, capsule, rigidbody] = capsuleGroup.get<TransformComponent, CapsuleColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PushTransform(capsule, transform, capsule.rigidBody);
					}


					PhysicsSystem::Update(fixedTimeStep);

					// Set entity values to Jolt transform.

					for (auto entity : boxGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, box, rigidbody] = boxGroup.get<TransformComponent, BoxColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PullTransform(box, transform);
					}

					for (auto entity : capsuleGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, capsule, rigidbody] = capsuleGroup.get<TransformComponent, CapsuleColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PullTransform(capsule, transform);
					}
					for (auto entity : sphereGroup)
					{
						Entity brEntity{ entity, this };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, sphere, rigidbody] = sphereGroup.get<TransformComponent, SphereColliderComponent, RigidBodyComponent>(entity);
						PhysicsSystem::PullTransform(sphere, transform);
					}

					while (!PhysicsSystem::GetCollisionEnterQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetCollisionEnterQueue().front();
						PhysicsSystem::GetCollisionEnterQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);
						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{

								script->OnCollisionEnter(entity2.GetComponent<IDComponent>().ID);

							}
						}

						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnCollisionEnter(entity1.GetComponent<IDComponent>().ID);

							}
						}
					}


					while (!PhysicsSystem::GetCollisionPersistQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetCollisionPersistQueue().front();
						PhysicsSystem::GetCollisionPersistQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);

						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{
								script->OnCollisionStay(entity2.GetComponent<IDComponent>().ID);
							}
						}
						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnCollisionStay(entity1.GetComponent<IDComponent>().ID);

							}
						}
					}

					while (!PhysicsSystem::GetCollisionExitQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetCollisionExitQueue().front();
						PhysicsSystem::GetCollisionExitQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);
						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{

								script->OnCollisionExit(entity2.GetComponent<IDComponent>().ID);

							}
						}

						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnCollisionExit(entity1.GetComponent<IDComponent>().ID);

							}
						}
					}


					while (!PhysicsSystem::GetTriggerEnterQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetTriggerEnterQueue().front();
						PhysicsSystem::GetTriggerEnterQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);
						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{

								script->OnTriggerEnter(collisionPair.second);

							}
						}

						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnTriggerEnter(collisionPair.first);

							}
						}
					}


					while (!PhysicsSystem::GetTriggerPersistQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetTriggerPersistQueue().front();
						PhysicsSystem::GetTriggerPersistQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);

						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{
								script->OnTriggerStay(entity2.GetComponent<IDComponent>().ID);
							}
						}
						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnTriggerStay(entity1.GetComponent<IDComponent>().ID);

							}
						}
					}

					while (!PhysicsSystem::GetTriggerExitQueue().empty())
					{
						auto collisionPair = PhysicsSystem::GetTriggerExitQueue().front();
						PhysicsSystem::GetTriggerExitQueue().pop();
						Entity entity1 = GetEntityByUUID(collisionPair.first);
						Entity entity2 = GetEntityByUUID(collisionPair.second);
						if (entity1.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent1 = entity1.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent1.mScripts)
							{

								script->OnTriggerExit(entity2.GetComponent<IDComponent>().ID);

							}
						}

						if (entity2.HasComponent<ScriptComponent>())
						{
							auto& scriptComponent2 = entity2.GetComponent<ScriptComponent>();
							for (auto& [name, script] : scriptComponent2.mScripts)
							{

								script->OnTriggerExit(entity1.GetComponent<IDComponent>().ID);

							}
						}
					}
				}

				

			

				for (auto entity : view)
				{
					Entity brEntity{ entity, this };
					if (!brEntity.IsActive())
					{
						continue;
					}
					auto& scriptComponent = view.get<ScriptComponent>(entity);
					for (auto& [name, script] : scriptComponent.mScripts)
					{
						if (script->IsActive())
						{
							script->LateUpdate();
						}
					}
				}
				if (PhysicsSystem::DebugDrawGet())
					PhysicsSystem::DrawDebug();
			}
		}

		ButtonSystem::Update();

		Camera* mainCamera = nullptr;
		glm::mat4 mainCameratransform(1.f);

		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, CameraComponent>);
			for (auto entity : group)
			{
				Entity brEntity{ entity, this };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					Entity brEntity{ entity, this };

					//camera.Camera.SetCameraType(SceneCamera::CameraType::Perspective);
					mainCamera = &camera.Camera;
					mainCameratransform = transform.GetGlobalTransform();
					break;
				}
			}
		}

		//// Pre-Render
		//if (mainCamera)
		//{
		//	auto viewMatrix = glm::inverse(mainCameratransform);
		//	auto projectionMatrix = mainCamera->GetProjectionMatrix();
		//	auto ProjectionViewMatrix = projectionMatrix * viewMatrix;
		//	Renderer3D::Begin(ProjectionViewMatrix);
		//	Render3DPass();
		//	Renderer3D::End();

		//	Renderer2D::Begin(ProjectionViewMatrix);
		//	Render2DPass();
		//	Renderer2D::End();
		//}
		

		//Audio
		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, AudioListenerComponent>);
			int listener = 0;
			for (auto& entity : group)
			{
				Entity brEntity{ entity, this };
				if (!brEntity.IsActive())
				{
					continue;
				}
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
					Entity brEntity{ entity, this };
					if (!brEntity.IsActive())
					{
						continue;
					}

					auto [transform, audio] = group.get<TransformComponent, AudioSourceComponent>(entity);
					if (audio.isPlaying && (!Borealis::AudioEngine::isSoundPlaying(audio.channelID) || !audio.isLoop))
					{
						AudioEngine::StopChannel(audio.channelID);
						audio.isPlaying = false;
						audio.channelID = Borealis::AudioEngine::PlayAudio(audio, transform.GetGlobalTranslate(), audio.Volume, audio.isMute, audio.isLoop, 0);
						//audio.channelID = Borealis::AudioEngine::PlayAudio(audio.audio->AudioPath, {}, audio.Volume, audio.isMute, audio.isLoop);
					}
				}
			}
		}

		//particles
		{
			entt::basic_group group = mRegistry.group<>(entt::get<TransformComponent, ParticleSystemComponent>);
			for (auto& entity : group)
			{
				auto entityBR = Entity{ entity, this };
				if (!entityBR.IsActive())
				{
					continue;
				}
				auto [transform, particleSystemComponent] = group.get<TransformComponent, ParticleSystemComponent>(entity);

				if (!particleSystemComponent.particleSystem)
				{
					particleSystemComponent.particleSystem = MakeRef<ParticleSystem>();
					particleSystemComponent.particleSystem->Init(particleSystemComponent);

					particleSystemComponent.texture = Texture2D::GetDefaultTexture();
				}

				particleSystemComponent.particleSystem->Update(particleSystemComponent, transform, dt);
			}
		}
	}

	//move down ltr
	Ref<FrameBuffer> Scene::GetRunTimeFB()
	{
		//move to rendergraph
		if (!mViewportFrameBuffer || !mRuntimeFrameBuffer || !mGFrameBuffer || !mPixelBuffer)
		{
			FrameBufferProperties props{ 1280, 720, false };
			props.Attachments = { FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
			mViewportFrameBuffer = FrameBuffer::Create(props);

			FrameBufferProperties propsRuntime{ 1280, 720, false };
			propsRuntime.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger,FramebufferTextureFormat::Depth };
			mRuntimeFrameBuffer = FrameBuffer::Create(propsRuntime);

			FrameBufferProperties propsGBuffer{ 1280, 720, false };
			propsGBuffer.Attachments =
			{
				FramebufferTextureFormat::RGBA16F,  // Albedo + Alpha
				FramebufferTextureFormat::RedInteger,  // entity id
				FramebufferTextureFormat::RGBA8,   // Normal + roughness
				FramebufferTextureFormat::RGBA8,   // Specular + metallic
				//FramebufferTextureFormat::RGB16F,   // Position
				FramebufferTextureFormat::Depth     // Depth buffer
			};
			mGFrameBuffer = FrameBuffer::Create(propsGBuffer);

			FrameBufferProperties propsShadowMapBuffer{ 2024, 2024, false };
			propsShadowMapBuffer.Attachments = { FramebufferTextureFormat::Depth };
			mShadowMapBuffer = FrameBuffer::Create(propsShadowMapBuffer);

			PixelBufferProperties propsPixelBuffer{ 1280, 720 };
			mPixelBuffer = PixelBuffer::Create(propsPixelBuffer);
		}
		return mRuntimeFrameBuffer;
	}

	Ref<FrameBuffer> Scene::GetEditorFB()
	{
		//move to rendergraph
		if (!mViewportFrameBuffer || !mRuntimeFrameBuffer || !mGFrameBuffer || !mPixelBuffer)
		{
			FrameBufferProperties props{ 1280, 720, false };
			props.Attachments = { FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
			mViewportFrameBuffer = FrameBuffer::Create(props);

			FrameBufferProperties propsRuntime{ 1280, 720, false };
			propsRuntime.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger,FramebufferTextureFormat::Depth };
			mRuntimeFrameBuffer = FrameBuffer::Create(propsRuntime);

			FrameBufferProperties propsGBuffer{ 1280, 720, false };
			propsGBuffer.Attachments =
			{
				FramebufferTextureFormat::RGBA16F,  // Albedo + Alpha
				FramebufferTextureFormat::RedInteger,  // entity id
				FramebufferTextureFormat::RGBA8,   // Normal + roughness
				FramebufferTextureFormat::RGBA8,   // Specular + metallic
				//FramebufferTextureFormat::RGB16F,   // Position
				FramebufferTextureFormat::Depth     // Depth buffer
			};
			mGFrameBuffer = FrameBuffer::Create(propsGBuffer);

			FrameBufferProperties propsShadowMapBuffer{ 2024, 2024, false };
			propsShadowMapBuffer.Attachments = { FramebufferTextureFormat::Depth };
			mShadowMapBuffer = FrameBuffer::Create(propsShadowMapBuffer);

			PixelBufferProperties propsPixelBuffer{ 1280, 720 };
			mPixelBuffer = PixelBuffer::Create(propsPixelBuffer);
		}
		return mViewportFrameBuffer;
	}

	Ref<PixelBuffer> Scene::GetPixelBuffer()
	{
		return mPixelBuffer;
	}

	void Scene::SetRenderGraphConfig(RenderGraphConfig renderGraphConfig)
	{
		mRenderGraph.SetConfig(renderGraphConfig);
	}

	void Scene::AddRenderGraphGlobalSource(Ref<RenderSource> globalSource)
	{
		mRenderGraph.SetGlobalSource(globalSource);
	}

	void Scene::ClearRenderGraph()
	{
		mRenderGraph.Init();
	}

	void Scene::UpdateEditor(float dt, EditorCamera& camera)
	{
		Renderer3D::Begin(camera);
		Render3DPass();
		Renderer3D::End();

		Renderer2D::Begin(camera);
		Render2DPass();
		Renderer2D::End();
	}

	void Scene::UpdateRenderer(float dt)
	{
		//move to rendergraph
		if (!mViewportFrameBuffer || !mRuntimeFrameBuffer || !mGFrameBuffer)
		{
			FrameBufferProperties props{ 1280, 720, false };
			props.Attachments = { FramebufferTextureFormat::RGBA16F,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
			mViewportFrameBuffer = FrameBuffer::Create(props);

			FrameBufferProperties propsRuntime{ 1280, 720, false };
			propsRuntime.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RedInteger,FramebufferTextureFormat::Depth };
			mRuntimeFrameBuffer = FrameBuffer::Create(propsRuntime);

			FrameBufferProperties propsGBuffer{ 1280, 720, false };
			propsGBuffer.Attachments =
			{
				FramebufferTextureFormat::RGBA16F,  // Albedo + Alpha
				FramebufferTextureFormat::RedInteger,  // entity id
				FramebufferTextureFormat::RGBA8,   // Normal + roughness
				FramebufferTextureFormat::RGBA8,   // Specular + metallic
				//FramebufferTextureFormat::RGB16F,   // Position
				FramebufferTextureFormat::Depth     // Depth buffer
			};
			mGFrameBuffer = FrameBuffer::Create(propsGBuffer);

			FrameBufferProperties propsShadowMapBuffer{ 2024, 2024,false };
			propsShadowMapBuffer.Attachments = { FramebufferTextureFormat::Depth };
			mShadowMapBuffer = FrameBuffer::Create(propsShadowMapBuffer);

			PixelBufferProperties propsPixelBuffer{ 1280, 720 };
			mPixelBuffer = PixelBuffer::Create(propsPixelBuffer);
		}

		Camera* mainCamera = nullptr; // camera not found
		glm::mat4 mainCameratransform(1.f);

		{
			auto group = mRegistry.group<>(entt::get<TransformComponent, CameraComponent>);
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					Entity brEntity { entity, this };
					//camera.Camera.SetCameraType(SceneCamera::CameraType::Perspective);
					mainCamera = &camera.Camera;
					mainCameratransform = transform.GetGlobalTransform();;

					//mainCameratransform = glm::translate(mainCameratransform, glm::vec3{0.f,0.01f,0.f});
					//transform.SetGlobalTransform(brEntity, mainCameratransform);

					break;
				}
			}
		}

		mRenderGraph.Init();

		RenderTargetSource editorBuffer("EditorBuffer", mViewportFrameBuffer);
		mRenderGraph.SetGlobalSource(MakeRef<RenderTargetSource>(editorBuffer));

		GBufferSource gBufferSource("gBuffer", mGFrameBuffer);
		mRenderGraph.SetGlobalSource(MakeRef<GBufferSource>(gBufferSource));

		RenderTargetSource shadowMapBuffer("ShadowMapBuffer", mShadowMapBuffer);
		mRenderGraph.SetGlobalSource(MakeRef<RenderTargetSource>(shadowMapBuffer));

		PixelBufferSource pixelBuffer("PixelBuffer", mPixelBuffer);
		mRenderGraph.SetGlobalSource(MakeRef<PixelBufferSource>(pixelBuffer));

		//PixelBufferSource nullPixelBuffer("NullPixelBuffer", nullptr);
		//mRenderGraph.SetGlobalSource(MakeRef<PixelBufferSource>(nullPixelBuffer));

		if (mainCamera)
		{
			RenderTargetSource runtimeBuffer("RunTimeBuffer", mRuntimeFrameBuffer);
			mRenderGraph.SetGlobalSource(MakeRef<RenderTargetSource>(runtimeBuffer));

			CameraSource runTimeCameraSource("RunTimeCamera", *mainCamera, mainCameratransform);
			mRenderGraph.SetGlobalSource(MakeRef<CameraSource>(runTimeCameraSource));
		}

		mRenderGraph.SetEntityRegistry(mRegistry);

		mRenderGraph.Finalize();

		mRenderGraph.SetFinalSink("BackBuffer", "Render2D.renderTarget"); //do i need it for immediate mode?

		mRenderGraph.Execute(dt);

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

		return {(entt::entity)-1, this};
	}
	void Scene::DestroyEntity(Entity entity)
	{		
		mEntityMap.erase(entity.GetUUID());
		if (hasRuntimeStarted)
		{
			if (entity.HasComponent<CharacterControllerComponent>())
			{
				PhysicsSystem::FreeCharacter(entity.GetComponent<CharacterControllerComponent>());
			}
			else if (entity.HasComponent<BoxColliderComponent>())
			{
				PhysicsSystem::FreeRigidBody(entity.GetComponent<BoxColliderComponent>());
				entity.GetComponent<BoxColliderComponent>().rigidBody = nullptr;
			}
			else if (entity.HasComponent<SphereColliderComponent>())
			{
				PhysicsSystem::FreeRigidBody(entity.GetComponent<SphereColliderComponent>());
				entity.GetComponent<SphereColliderComponent>().rigidBody = nullptr;
			}
			else if (entity.HasComponent<CapsuleColliderComponent>())
			{
				PhysicsSystem::FreeRigidBody(entity.GetComponent<CapsuleColliderComponent>());
				entity.GetComponent<CapsuleColliderComponent>().rigidBody = nullptr;
			}
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
	}

	template <>
	static void CopyComponent<ScriptComponent>(Entity dst, Entity src)
	{
		if (src.HasComponent<ScriptComponent>())
		{
			dst.AddOrReplaceComponent<ScriptComponent>(src.GetComponent<ScriptComponent>());


			auto& srcComponent = src.GetComponent<ScriptComponent>();

			auto& newScriptComponent = dst.GetComponent<ScriptComponent>();

			for (auto script : srcComponent.mScripts)
			{
				Ref<ScriptInstance> newScript = MakeRef<ScriptInstance>(script.second->GetScriptClass());
				newScript->Init(dst.GetComponent<IDComponent>().ID);
				newScriptComponent.AddScript(script.first, newScript);
			}

			// Initialize details
			auto& dstScriptComponent = newScriptComponent;
			auto& srcScriptComponent = srcComponent;

			for (auto dstIT = dstScriptComponent.mScripts.begin(); dstIT != dstScriptComponent.mScripts.end(); dstIT++)
			{
				auto scriptKlass = dstIT->second->GetScriptClass();
				auto srcIT = srcScriptComponent.mScripts.find(dstIT->first);
				for (auto property : scriptKlass->mFields)
				{
					dstIT->second->ReplaceFieldValue(srcIT->second.get(), property.first);
				}
			}
		}

	}

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		name+= " (clone)";
		Entity newEntity = CreateEntity(name);
		CopyComponent<TagComponent>(newEntity, entity);
		newEntity.GetName() = name;
		CopyComponent<TransformComponent>(newEntity, entity);
		CopyComponent<SpriteRendererComponent>(newEntity, entity);
		CopyComponent<CameraComponent>(newEntity, entity);
		CopyComponent<NativeScriptComponent>(newEntity, entity);
		CopyComponent<MeshFilterComponent>(newEntity,entity);
		CopyComponent<MeshRendererComponent>(newEntity,entity);
		CopyComponent<SkinnedMeshRendererComponent>(newEntity,entity);
		CopyComponent<AnimatorComponent>(newEntity,entity);
		CopyComponent<BoxColliderComponent>(newEntity,entity);
		CopyComponent<CapsuleColliderComponent>(newEntity,entity);
		CopyComponent<RigidBodyComponent>(newEntity, entity);
		CopyComponent<CharacterControllerComponent>(newEntity, entity);
		CopyComponent<LightComponent>(newEntity, entity);
		CopyComponent<CircleRendererComponent>(newEntity, entity);
		CopyComponent<TextComponent>(newEntity, entity);
		CopyComponent<AudioSourceComponent>(newEntity, entity);
		CopyComponent<AudioListenerComponent>(newEntity, entity);
		CopyComponent<ScriptComponent>(newEntity, entity);
		CopyComponent<BehaviourTreeComponent>(newEntity, entity);
		CopyComponent<OutLineComponent>(newEntity, entity);
		CopyComponent<CanvasComponent>(newEntity, entity);
		CopyComponent<CanvasRendererComponent>(newEntity, entity);
		CopyComponent<ParticleSystemComponent>(newEntity, entity);
		CopyComponent<ButtonComponent>(newEntity, entity);
		auto& tc = newEntity.GetComponent<TransformComponent>();
		if (tc.ParentID)
		{
			auto parent = GetEntityByUUID(tc.ParentID);
			auto& parentTC = parent.GetComponent<TransformComponent>();
			parentTC.ChildrenID.insert(newEntity.GetComponent<IDComponent>().ID);
		}
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
		std::unordered_map<UUID, entt::entity> srcMap;
		auto view = src.view<ScriptComponent>();
		for (auto srcEntity : view)
		{
			UUID uuid = src.get<IDComponent>(srcEntity).ID;
			srcMap[uuid] = srcEntity;
			auto dstEntity = entitymap.at(uuid);

			auto srcComponent = view.get<ScriptComponent>(srcEntity);

			auto& newScriptComponent = dst.emplace<ScriptComponent>(dstEntity);

			for (auto script : srcComponent.mScripts)
			{
				Ref<ScriptInstance> newScript = MakeRef<ScriptInstance>(script.second->GetScriptClass());
				newScript->Init(uuid);
				newScriptComponent.AddScript(script.first, newScript);
			}

			// Initialize details
			auto& dstScriptComponent = newScriptComponent;
			auto& srcScriptComponent = srcComponent;

			for (auto dstIT = dstScriptComponent.mScripts.begin(); dstIT != dstScriptComponent.mScripts.end(); dstIT++)
			{
				auto scriptKlass = dstIT->second->GetScriptClass();
				auto srcIT = srcScriptComponent.mScripts.find(dstIT->first);
				for (auto property : scriptKlass->mFields)
				{
					if (property.second.isNativeComponent() && 
						((!property.second.hasHideInInspector(scriptKlass->GetMonoClass()) && property.second.isPublic()) || 
							(property.second.hasSerializeField(scriptKlass->GetMonoClass()))))
					{
						MonoObject* scriptReference = srcIT->second->GetFieldValue<MonoObject*>(property.first);
						UUID scriptUUID = property.second.GetGameObjectID(scriptReference);
						MonoObject* data;
						InitGameObject(data , scriptUUID, property.second.mFieldClassName());
						dstIT->second->SetFieldValue(property.first, data);
					}
					else if (!property.second.isMonoBehaviour() && !property.second.isGameObject() &&
						((!property.second.hasHideInInspector(scriptKlass->GetMonoClass()) && property.second.isPublic())
							|| (property.second.hasSerializeField(scriptKlass->GetMonoClass())))
						)

					dstIT->second->ReplaceFieldValue(srcIT->second.get(), property.first);
				}
			}
		}

		auto dstView = dst.view<ScriptComponent>();
		for (auto dstEntity : dstView)
		{
			auto dstScriptComponent = dstView.get<ScriptComponent>(dstEntity);
			UUID dstID = dst.get<IDComponent>(dstEntity).ID;
			entt::entity dstEnttID = srcMap.at(dstID);
			auto srcScriptComponent = view.get<ScriptComponent>(dstEnttID);
			auto srcIT = srcScriptComponent.mScripts.begin();
			for (auto dstIT = dstScriptComponent.mScripts.begin(); dstIT != dstScriptComponent.mScripts.end(); dstIT++, srcIT++)
			{
				auto scriptKlass = dstIT->second->GetScriptClass();
				for (auto property : scriptKlass->mFields)
				{
					if (property.second.isMonoBehaviour() &&
						((!property.second.hasHideInInspector(scriptKlass->GetMonoClass()) && property.second.isPublic())
							|| (property.second.hasSerializeField(scriptKlass->GetMonoClass())))
						)
					{

						MonoObject* Data = srcIT->second->GetFieldValue<MonoObject*>(property.first);

						UUID monoBehaviourEntityID = property.second.GetAttachedID(Data);
						BOREALIS_CORE_ASSERT(monoBehaviourEntityID != 0, "UUID is 0");

						if (entitymap.find(monoBehaviourEntityID) != entitymap.end())
						{
							auto monoBehaviourEntity = entitymap.at(monoBehaviourEntityID);
							// find script in monobehaviourEntity
							ScriptComponent& targetScriptComp = dst.get<ScriptComponent>(monoBehaviourEntity);

							auto scriptTarget = targetScriptComp.mScripts.find(scriptKlass->mFields[property.first].mFieldClassName());

							dstIT->second->SetFieldValue(property.first, scriptTarget->second->GetInstance());
						}
					}

					if (property.second.isGameObject() &&
						((!property.second.hasHideInInspector(scriptKlass->GetMonoClass()) && property.second.isPublic())
							|| (property.second.hasSerializeField(scriptKlass->GetMonoClass())))
						)

					{
						MonoObject* DstData = dstIT->second->GetFieldValue<MonoObject*>(property.first);
						MonoObject* Data = srcIT->second->GetFieldValue<MonoObject*>(property.first);
						if (!Data) continue;
						UUID setUUID = property.second.GetGameObjectID(Data);
						BOREALIS_CORE_ASSERT(setUUID != 0, "UUID is 0");
						InitGameObject(DstData, setUUID, property.second.mFieldClassName());
						dstIT->second->SetFieldValue(property.first, DstData);
					}
				}
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
			const auto& name = originalRegistry.get<TagComponent>(entity).Name;
			UUIDtoENTT[uuid] = newScene->CreateEntityWithUUID(name, uuid);
			Entity newEntity = newScene->GetEntityByUUID(uuid);
			newEntity.GetComponent<TagComponent>().Tag = originalRegistry.get<TagComponent>(entity).Tag;
			newEntity.GetComponent<TagComponent>().active = originalRegistry.get<TagComponent>(entity).active;
			newEntity.GetComponent<TagComponent>().mLayer = originalRegistry.get<TagComponent>(entity).mLayer;
		}
		CopyComponent<TransformComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<SpriteRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CameraComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<NativeScriptComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<MeshFilterComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<MeshRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<SkinnedMeshRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<AnimatorComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<BoxColliderComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CapsuleColliderComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<RigidBodyComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<LightComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CharacterControllerComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CircleRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<TextComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<AudioSourceComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<AudioListenerComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<ScriptComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<BehaviourTreeComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<OutLineComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CanvasComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<CanvasRendererComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<ParticleSystemComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		CopyComponent<ButtonComponent>(newRegistry, originalRegistry, UUIDtoENTT);
		auto tcView = newRegistry.view<TransformComponent>();
		for (auto entity : tcView)
		{
			auto name = newRegistry.get<TagComponent>(entity).Name;
			auto& tc = tcView.get<TransformComponent>(entity);
		}

		return newScene;
	}

	void Scene::RuntimeStart()
	{
		hasRuntimeStarted = true;

		auto boxGroup = mRegistry.group<>(entt::get<TransformComponent, BoxColliderComponent>);
		for (auto entity : boxGroup)
		{
			Entity brEntity{ entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto [transform, box] = boxGroup.get<TransformComponent, BoxColliderComponent>(entity);
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			if (mRegistry.storage<RigidBodyComponent>().contains(entity))
			{
				PhysicsSystem::addBody(transform, &mRegistry.get<RigidBodyComponent>(entity), box, entityID);
				box.rigidBody = &mRegistry.get<RigidBodyComponent>(entity);
			}
			else if (!mRegistry.storage<CharacterControllerComponent>().contains(entity))
			{
				PhysicsSystem::addBody(transform, nullptr, box, entityID);
			}
		}

		auto sphereGroup = mRegistry.group<>(entt::get<TransformComponent, SphereColliderComponent>);
		for (auto entity : sphereGroup)
		{
			Entity brEntity{ entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto [transform, sphere] = sphereGroup.get<TransformComponent, SphereColliderComponent>(entity);
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			if (mRegistry.storage<RigidBodyComponent>().contains(entity))
			{
				PhysicsSystem::addBody(transform, &mRegistry.get<RigidBodyComponent>(entity), sphere, entityID);
				sphere.rigidBody = &mRegistry.get<RigidBodyComponent>(entity);
			}
			else
			{
				PhysicsSystem::addBody(transform, nullptr, sphere, entityID);
			}
		}

		auto capsuleGroup = mRegistry.group<>(entt::get<TransformComponent, CapsuleColliderComponent>);
		for (auto entity : capsuleGroup)
		{
			Entity brEntity { entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto [transform, capsule] = capsuleGroup.get<TransformComponent, CapsuleColliderComponent>(entity);
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			if (mRegistry.storage<RigidBodyComponent>().contains(entity))
			{
				PhysicsSystem::addBody(transform, &mRegistry.get<RigidBodyComponent>(entity), capsule, entityID);
				capsule.rigidBody = &mRegistry.get<RigidBodyComponent>(entity);
			}
			else
			{
				PhysicsSystem::addBody(transform, nullptr, capsule, entityID);
			}
		}

		auto CapsulecharacterGroup = mRegistry.group<>(entt::get<TransformComponent, CharacterControllerComponent, CapsuleColliderComponent>);
		for (auto entity : CapsulecharacterGroup)
		{
			Entity brEntity{ entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			PhysicsSystem::addCharacter(brEntity.GetComponent<CharacterControllerComponent>(), brEntity.GetComponent<TransformComponent>(), brEntity.GetComponent<CapsuleColliderComponent>(), entityID);
		}
		auto BoxcharacterGroup = mRegistry.group<>(entt::get<TransformComponent, CharacterControllerComponent, BoxColliderComponent>);
		for (auto entity : BoxcharacterGroup)
		{
			Entity brEntity{ entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			PhysicsSystem::addCharacter(brEntity.GetComponent<CharacterControllerComponent>(), brEntity.GetComponent<TransformComponent>(), brEntity.GetComponent<BoxColliderComponent>(), entityID);
		}
		auto SpherecharacterGroup = mRegistry.group<>(entt::get<TransformComponent, CharacterControllerComponent, SphereColliderComponent>);
		for (auto entity : SpherecharacterGroup)
		{
			Entity brEntity{ entity, this };
			if (!brEntity.IsActive())
			{
				continue;
			}
			auto entityID = mRegistry.get<IDComponent>(entity).ID;
			PhysicsSystem::addCharacter(brEntity.GetComponent<CharacterControllerComponent>(), brEntity.GetComponent<TransformComponent>(), brEntity.GetComponent<SphereColliderComponent>(), entityID);
		}

		auto IDView = mRegistry.view<IDComponent>();
		for (auto entity : IDView)
		{
			LayerList::initializeEntity({ entity,this });
		}

		auto scriptGroup = mRegistry.group<>(entt::get<ScriptComponent>);

		for (auto entity : scriptGroup)
		{
			Entity brEntity{ entity, this };
			auto& scriptComponent = scriptGroup.get<ScriptComponent>(entity);
			for (auto& [name, script] : scriptComponent.mScripts)
			{
				script->Awake();
			}

		}

		for (auto entity : scriptGroup)
		{
			Entity brEntity{ entity, this };
			auto& scriptComponent = scriptGroup.get<ScriptComponent>(entity);
			for (auto& [name, script] : scriptComponent.mScripts)
			{
				script->Start();
			}
		
		}

		auto behaviourTreeGroup = mRegistry.group<>(entt::get<TransformComponent, BehaviourTreeComponent>);
		for (auto entity : behaviourTreeGroup)
		{
			auto [transform, btree] = behaviourTreeGroup.get<TransformComponent, BehaviourTreeComponent>(entity);
			auto entityID = mRegistry.get<IDComponent>(entity).ID;

			//BTreeFactory::Instance().PrintTree(btree.mBehaviourTreeData, btree.mBehaviourTreeData->RootNodeID);
			if (btree.mBehaviourTreeData)
			{
				// Build the behavior tree
				BehaviourNode rootNode{};
				BTreeFactory::Instance().BuildBehaviourTreeFromData(btree.mBehaviourTreeData, rootNode);
				if (!btree.mBehaviourTrees) {
					btree.mBehaviourTrees = MakeRef<BehaviourTree>(); // or std::shared_ptr, depending on its type
				}
				// Assign to btree.mBehaviourTrees
				btree.mBehaviourTrees->SetRootNode(rootNode);
			}
			else
			{
				BOREALIS_CORE_ERROR("BehaviourTreeData is null for entity {}", entityID);
			}
		}

	}

	void Scene::RuntimeEnd()
	{
		hasRuntimeStarted = false;

		{
			auto characterView = mRegistry.view<CharacterControllerComponent>();
			for (auto entity : characterView)
			{
				PhysicsSystem::FreeCharacter(characterView.get<CharacterControllerComponent>(entity));
			}
		}

		{
			auto boxView = mRegistry.view<BoxColliderComponent>();
			for (auto entity : boxView)
			{
				if (!mRegistry.storage<CharacterControllerComponent>().contains(entity))
				PhysicsSystem::FreeRigidBody(boxView.get<BoxColliderComponent>(entity));
			}
		}

		{
			auto capsuleView = mRegistry.view<CapsuleColliderComponent>();
			for (auto entity : capsuleView)
			{
				if (!mRegistry.storage<CharacterControllerComponent>().contains(entity))
				PhysicsSystem::FreeRigidBody(capsuleView.get<CapsuleColliderComponent>(entity));
			}
		}

		{
			auto sphereView = mRegistry.view<SphereColliderComponent>();
			for (auto entity : sphereView)
			{
				if (!mRegistry.storage<CharacterControllerComponent>().contains(entity))
				PhysicsSystem::FreeRigidBody(sphereView.get<SphereColliderComponent>(entity));
			}
		}
		PhysicsSystem::EndScene();
		PhysicsSystem::GetCollisionEnterQueue() = std::queue<CollisionPair>();
		PhysicsSystem::GetCollisionPersistQueue() = std::queue<CollisionPair>();
		PhysicsSystem::GetCollisionExitQueue() = std::queue<CollisionPair>();
		PhysicsSystem::GetTriggerEnterQueue() = std::queue<CollisionPair>();
		PhysicsSystem::GetTriggerPersistQueue() = std::queue<CollisionPair>();
		PhysicsSystem::GetTriggerExitQueue() = std::queue<CollisionPair>();

		LayerList::resetEntities();
		AudioEngine::StopAllChannels();
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
	void Scene::OnComponentAdded<SkinnedMeshRendererComponent>(Entity entity, SkinnedMeshRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<AnimatorComponent>(Entity entity, AnimatorComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component)
	{
		if (entity.HasComponent<MeshFilterComponent>())
		{
			if (entity.GetComponent<MeshFilterComponent>().Model)
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).first;
				component.size = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).second;
			}
		}
		else if (entity.HasComponent<SkinnedMeshRendererComponent>())
		{
			if (entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel)
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).first;
				component.size = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).second;
			}
		}
		else
		{
			component.center = { 0,0,0 };
			component.size = { 1,1,1 };
		}
	}
	template<>
	void Scene::OnComponentAdded<CapsuleColliderComponent>(Entity entity, CapsuleColliderComponent& component)
	{
		if (entity.HasComponent<MeshFilterComponent>())
		{
			if (entity.GetComponent<MeshFilterComponent>().Model)
			
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).first;
				glm::vec3 data = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).second;
				component.radius = PhysicsSystem::calculateCapsuleDimensions(data).first;
				component.height = PhysicsSystem::calculateCapsuleDimensions(data).second;
			}
		}
		else if (entity.HasComponent<SkinnedMeshRendererComponent>())
		{
			if (entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel)
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).first;
				glm::vec3 data = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).second;
				component.radius = PhysicsSystem::calculateCapsuleDimensions(data).first;
				component.height = PhysicsSystem::calculateCapsuleDimensions(data).second;
			}
		}
		else
		{
			component.radius = 1.f;
			component.height = 2.f;
			component.center = { 0,0,0 };
		}
	}

	template<>
	void Scene::OnComponentAdded<SphereColliderComponent>(Entity entity, SphereColliderComponent& component)
	{
		if (entity.HasComponent<MeshFilterComponent>())
		{
			if (entity.GetComponent<MeshFilterComponent>().Model)
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).first;
				glm::vec3 data = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<MeshFilterComponent>().Model.get())).second;
				component.radius = PhysicsSystem::calculateSphereRadius(data);
			}
		}
		else if (entity.HasComponent<SkinnedMeshRendererComponent>())
		{
			if (entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel)
			{
				component.center = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).first;
				glm::vec3 data = PhysicsSystem::calculateBoundingVolume(*(entity.GetComponent<SkinnedMeshRendererComponent>().SkinnnedModel.get())).second;
				component.radius = PhysicsSystem::calculateSphereRadius(data);
			}
		}
		else
		{
			component.center = { 0,0,0 };
			component.radius = 1.f;
		}
	}

	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CharacterControllerComponent>(Entity entity, CharacterControllerComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{
		component.font = Font::GetDefaultFont();
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

	template<>
	void Scene::OnComponentAdded<OutLineComponent>(Entity entity, OutLineComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CanvasComponent>(Entity entity, CanvasComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CanvasRendererComponent>(Entity entity, CanvasRendererComponent& component)
	{

	}	

	template<>
	void Scene::OnComponentAdded<ParticleSystemComponent>(Entity entity, ParticleSystemComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<ButtonComponent>(Entity entity, ButtonComponent& component)
	{
		
	}
}
