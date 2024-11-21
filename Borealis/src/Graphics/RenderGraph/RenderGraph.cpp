/******************************************************************************
/*!
\file       RenderGraph.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 16, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/RenderGraph/RenderGraph.hpp>

#include <Assets/AssetManager.hpp>
#include <Graphics/Frustum.hpp>
#include <Graphics/Renderer3D.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Graphics/RenderCommand.hpp>
#include <Scene/Components.hpp>
#include <Scene/SceneManager.hpp>

#include <Graphics/PixelBuffer.hpp>

namespace Borealis
{
	Ref<Shader> s_shader = nullptr;
	Ref<Shader> material_shader = nullptr;
	Ref<Shader> cascade_shadow_shader = nullptr;
	Ref<Shader> common_shader = nullptr;
	Ref<Shader> quad_shader = nullptr;


	Ref<FrameBuffer> mCascadeShadowMapBuffer = nullptr;

	//========================================================================
	//BUFFER SOURCE
	//========================================================================
	RenderTargetSource::RenderTargetSource(std::string name, Ref<FrameBuffer> framebuffer)
	{
		sourceName = name;
		sourceType = RenderSourceType::RenderTargetColor;

		buffer = framebuffer;

		Width = framebuffer->GetProperties().Width;
		Height = framebuffer->GetProperties().Height;
	}
	
	BoolSource::BoolSource(std::string name, bool ref) : mRef(ref)
	{
		sourceName = name;
		sourceType = RenderSourceType::Bool;
	}

	void BoolSource::Bind() {};
	void BoolSource::Unbind() {};

	IntSource::IntSource(std::string name, int& ref) : mRef(ref)
	{
		sourceName = name;
		sourceType = RenderSourceType::IntRef;
	}

	void IntSource::Bind() {};
	void IntSource::Unbind() {};

	Vec2IntSource::Vec2IntSource(std::string name, int refX, int refY) : mRefX(refX), mRefY(refY)
	{
		sourceName = name;
		sourceType = RenderSourceType::Vec2Int;
	}

	void Vec2IntSource::Bind() {};
	void Vec2IntSource::Unbind() {};

	IntListSource::IntListSource(std::string name, std::list<int> const& intList) : mList(intList)
	{
		sourceName = name;
		sourceType = RenderSourceType::IntList;
	}

	void IntListSource::Bind() {};

	void IntListSource::Unbind() {};

	void RenderTargetSource::Bind()
	{
		if (buffer)
			buffer->Bind();
	}

	void RenderTargetSource::Unbind()
	{
		if (buffer)
			buffer->Unbind();
	}

	void RenderTargetSource::BindDepthBuffer(int index, bool is3D)
	{
		buffer->BindDepthBuffer(index, is3D);
	}

	GBufferSource::GBufferSource(std::string name, Ref<FrameBuffer> framebuffer)
	{
		sourceName = name;
		sourceType = RenderSourceType::GBuffer;

		buffer = framebuffer;
	}

	void GBufferSource::Bind()
	{
		if (buffer)
			buffer->Bind();
	}

	void GBufferSource::Unbind()
	{
		if (buffer)
			buffer->Unbind();
	}

	void GBufferSource::BindTexture(TextureType type, int index)
	{
		buffer->BindTexture(type, index);
	}

	void GBufferSource::BindDepthBuffer(int index)
	{
		buffer->BindDepthBuffer(index);
	}

	PixelBufferSource::PixelBufferSource(std::string name, Ref<PixelBuffer> pixelbuffer) : Width(0), Height(0)
	{
		sourceName = name;
		sourceType = RenderSourceType::PixelBuffer;

		buffer = pixelbuffer;
		if(buffer)
		{
			Width = pixelbuffer->GetProperties().Width;
			Height = pixelbuffer->GetProperties().Height;
		}
	}

	void PixelBufferSource::ReadTexture(uint32_t index)
	{
		buffer->ReadTexture(index);
	}

	void PixelBufferSource::Bind()
	{
		buffer->Bind();
	}

	void PixelBufferSource::Unbind()
	{
		buffer->Unbind();
	}

	void PixelBufferSource::Resize(uint32_t width, uint32_t height)
	{
		buffer->Resize(width, height);
	}

	CameraSource::CameraSource(std::string name, EditorCamera const& camera)
	{
		sourceName = name;
		sourceType = RenderSourceType::Camera;

		projMtx = camera.GetProjectionMatrix();
		viewMtx = camera.GetViewMatrix();
		viewProj = camera.GetViewProjectionMatrix();
		viewPortSize = camera.GetViewPortSize();
		position = camera.GetPosition();
		lookAt = camera.GetForwardDirection();
		editor = true;
		fov = camera.GetFOV();
		nearPlane = camera.GetNearPlane();
		farPlane = camera.GetFarPlane();
		aspectRatio = camera.GetAspectRatio();
	}

	CameraSource::CameraSource(std::string name, const Camera& camera, const glm::mat4& transform)
	{
		sourceName = name;
		sourceType = RenderSourceType::Camera;

		projMtx = camera.GetProjectionMatrix();
		viewMtx = glm::inverse(transform);
		viewProj = camera.GetProjectionMatrix() * glm::inverse(transform);
		viewPortSize = camera.GetViewPortSize();
		position = transform[3];
		lookAt = -glm::normalize(glm::vec3(transform[2]));

		editor = false;

		fov = camera.GetFOV();
		nearPlane = camera.GetNearPlane();
		farPlane = camera.GetFarPlane();
	}

	glm::mat4 CameraSource::GetViewProj()
	{
		return viewProj;
	}

	glm::vec2 CameraSource::GetViewPortSize()
	{
		return viewPortSize;
	}

	//========================================================================
	//RENDERPASS
	//========================================================================
	RenderPass::RenderPass(std::string name)
	{
		passName = name;
	}

	void RenderPass::SetSinkLinkage(std::string sinkName, std::string sourceName)
	{
		RenderSink sink;
		sink.sinkName = sinkName;
		sink.outputName = passName + "." + sinkName;
		sink.sourceName = sourceName;

		sinkList.push_back(MakeRef<RenderSink>(sink));
	}

	void RenderPass::Bind()
	{
		if (shader) shader->Bind();
		for (auto sink : sinkList)
		{
			if (sink->source)
			{
				auto source = sink->source;
				source->Bind();
			}
		}
	}

	void RenderPass::Unbind()
	{
		for (auto sink : sinkList)
		{
			if (sink->source)
			{
				auto source = sink->source;
				source->Unbind();
			}
		}
		if (shader) shader->Unbind();
	}

	//========================================================================
	//ENTITY RENDERPASS
	//========================================================================
	void EntityPass::SetEntityRegistry(entt::registry& registry)
	{
		registryPtr = &registry;
	}

	//========================================================================
	//RENDER3D RENDERPASS
	//========================================================================

	Render3D::Render3D(std::string name) : EntityPass(name)
	{
		shader = nullptr;
	}

	glm::mat4 GetLightViewProj(LightComponent const& lightComponent, Ref<CameraSource> camera, float farPlane)
	{
		const glm::mat4 proj = glm::perspective(
			glm::radians(camera->fov), camera->aspectRatio, 
			camera->nearPlane,
			farPlane);

		glm::vec3 upVector = { 0.f,1.f,0.f };

		Frustum frustum = ComputeFrustum(proj * camera->viewMtx);
		FrustumCorners frustumCorners = GetCorners(frustum);
		FrustumCorners lightSpaceFrustumCorners = frustumCorners;
		AABB centerAABB = lightSpaceFrustumCorners.GetAABB();
		glm::vec3 center = (centerAABB.maxExtent + centerAABB.minExtent) * 0.5f;
		glm::mat4 lightView = glm::lookAt(center, center + lightComponent.direction, upVector);
		lightSpaceFrustumCorners.Transform(lightView);
		AABB aabb = lightSpaceFrustumCorners.GetAABB();

		constexpr float zMult = 10.0f;
		if (aabb.minExtent.z < 0)
		{
			aabb.minExtent.z *= zMult;
		}
		else
		{
			aabb.minExtent.z /= zMult;
		}
		if (aabb.maxExtent.z < 0)
		{
			aabb.maxExtent.z /= zMult;
		}
		else
		{
			aabb.maxExtent.z *= zMult;
		}

		glm::mat4 lightProjection = glm::ortho(aabb.minExtent.x, aabb.maxExtent.x, aabb.minExtent.y, aabb.maxExtent.y, aabb.minExtent.z, aabb.maxExtent.z);


		return lightProjection * lightView;
	}

	void SetShadowVariable(LightComponent const& lightComponent, Ref<Shader> shader, Ref<CameraSource> camera)
	{
		if (lightComponent.type == LightComponent::Type::Spot)
		{
			//need to change exact same code on top, fix next time
			glm::vec3 upVector = (glm::abs(lightComponent.direction.y) > 0.99f) ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 1.f, 0.f);
			glm::mat4 lightView = glm::lookAt(lightComponent.position, lightComponent.position + lightComponent.direction, upVector);
			float fieldOfView = glm::radians(lightComponent.spotAngle * 2.f); // Spotlight cone angle
			glm::mat4 lightProj = glm::perspective(fieldOfView, 1.f, 1.f, lightComponent.range); //change in the future

			shader->Set("u_LightViewProjection", lightProj * lightView);
		}
		else if (lightComponent.type == LightComponent::Type::Directional)
		{
			std::vector<float> shadowCascadeLevels{ camera->farPlane / 50.0f, camera->farPlane / 25.0f, camera->farPlane / 10.0f, camera->farPlane / 2.0f };

			std::array<glm::mat4, 4> lightViewProjMatrices;

			for (int i{}; i < lightViewProjMatrices.size(); ++i)
			{
				lightViewProjMatrices[i] = GetLightViewProj(lightComponent, camera, shadowCascadeLevels[i]);

				std::string str = "u_LightSpaceMatrices[" + std::to_string(i) + "]";
				shader->Set(str.c_str(), lightViewProjMatrices[i]);
			}

			for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
			{
				std::string str = "u_CascadePlaneDistances[" + std::to_string(i) + "]";
				shader->Set(str.c_str(), shadowCascadeLevels[i]);
			}

			shader->Set("u_View", camera->viewMtx);

			shader->Set("cascadeCount", 4);

			cascade_shadow_shader->Bind();

			for (int i{}; i < lightViewProjMatrices.size(); ++i)
			{
				std::string str = "u_LightSpaceMatrices[" + std::to_string(i) + "]";
				cascade_shadow_shader->Set(str.c_str(), lightViewProjMatrices[i]);
			}

			cascade_shadow_shader->Unbind();
		}
	}

	void SetShadowAndLight(Ref<RenderTargetSource> renderTarget, Ref<RenderTargetSource> shadowMap, Ref<Shader> shader,  entt::registry* registryPtr, Ref<CameraSource> camera)
	{
		//add light to light engine and shadow pass
		{
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				Renderer3D::AddLight(lightComponent);

				SetShadowVariable(lightComponent, shader, camera);

				shader->Bind();
				if (lightComponent.type == LightComponent::Type::Spot)
				{
					if (shadowMap)
					{
						shadowMap->BindDepthBuffer(0);
						shader->Set("u_ShadowMap", 0);
					}
				}
				else if(lightComponent.type == LightComponent::Type::Directional)
				{
					if (mCascadeShadowMapBuffer)
					{
						mCascadeShadowMapBuffer->BindDepthBuffer(0, true);
						shader->Set("u_CascadeShadowMap", 0);
					}
				}

				shader->Set("shadowPass", false);

				break; //TODO, for now 1 shadow
			}
		}
	}
	
	void Render3D::Execute(float dt)
	{
		glm::mat4 viewProjMatrix{};
		bool editor{};

		Ref<CameraSource> camera = nullptr;
		Ref<RenderTargetSource> renderTarget = nullptr;
		Ref<RenderTargetSource> shadowMap = nullptr;
		Ref<PixelBufferSource> pixelBuffer = nullptr;
		glm::vec3 camPos{};

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj();
				editor = std::dynamic_pointer_cast<CameraSource>(sink->source)->editor;
				camPos = std::dynamic_pointer_cast<CameraSource>(sink->source)->position;

				camera = std::dynamic_pointer_cast<CameraSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if(sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
					renderTarget->buffer->ClearAttachment(1, -1);
				}

				if (sink->sinkName == "shadowMap")
				{
					shadowMap = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}
		}
		//mesh pass
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);


				if (!meshFilter.Model || !meshRenderer.Material) continue;

				Frustum frustum = ComputeFrustum(viewProjMatrix);
				BoundingSphere modelBoundingSphere = meshFilter.Model->mBoundingSphere;
				modelBoundingSphere.Transform(TransformComponent::GetGlobalTransform(brEntity));


				if (CullBoundingSphere(frustum, modelBoundingSphere))
				{
					//BOREALIS_CORE_INFO("Culling entity {}", (int)entity);
					continue;
				}

				Ref<Shader> materialShader = meshRenderer.Material->GetShader();

				Renderer3D::Begin(viewProjMatrix, materialShader);


				SetShadowAndLight(renderTarget, shadowMap, materialShader, registryPtr, camera);
				renderTarget->Bind();
				Renderer3D::SetLights(materialShader);
				Renderer3D::DrawMesh(TransformComponent::GetGlobalTransform(brEntity), meshFilter, meshRenderer, materialShader, (int)entity);

				if(Renderer3D::GetGlobalWireFrameMode())
				{
					AABB modelAABB = meshFilter.Model->mAABB;
					modelAABB.Transform(TransformComponent::GetGlobalTransform(brEntity));
					if (brEntity.HasComponent<RigidBodyComponent>())
					{
						auto& rigidbody = brEntity.GetComponent<RigidBodyComponent>();
						rigidbody.minExtent = modelAABB.minExtent;
						rigidbody.maxExtent = modelAABB.maxExtent;
						Renderer3D::DrawCube(transform.Translate, rigidbody.minExtent, rigidbody.maxExtent, { 0.f,1.f,0.f,1.f }, true);
					}
				}

			}
		}

		//skinned mesh pass
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, SkinnedMeshRendererComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, skinnedMesh] = group.get<TransformComponent, SkinnedMeshRendererComponent>(entity);

				if (!skinnedMesh.SkinnnedModel || !skinnedMesh.Material) continue;

				Frustum frustum = ComputeFrustum(viewProjMatrix);

				Ref<Shader> materialShader = skinnedMesh.Material->GetShader();

				Renderer3D::Begin(viewProjMatrix, materialShader);

				materialShader->Bind();
				materialShader->Set("u_HasAnimation", false);

				SetShadowAndLight(renderTarget, shadowMap, materialShader, registryPtr, camera);
				renderTarget->Bind();
				materialShader->Bind();
				if (registryPtr->storage<AnimatorComponent>().contains(entity))
				{
					AnimatorComponent& animatorComponent = registryPtr->get<AnimatorComponent>(entity);

					if (animatorComponent.animation && !animatorComponent.animator.HasAnimation())
					{
						animatorComponent.animator.PlayAnimation(animatorComponent.animation);
						skinnedMesh.SkinnnedModel->AssignAnimation(animatorComponent.animation);
					}

					if (animatorComponent.animation)
					{
						animatorComponent.animator.UpdateAnimation(dt);

						if (skinnedMesh.SkinnnedModel->mAnimation)
						{
							materialShader->Set("u_HasAnimation", true);
							auto transforms = animatorComponent.animator.GetFinalBoneMatrices();
							for (int i = 0; i < transforms.size(); ++i)
							{
								std::string str = "u_FinalBonesMatrices[" + std::to_string(i) + "]";
								materialShader->Set(str.c_str(), transforms[i]);
							}
						}
					}
				}

				//Frustum frustum = ComputeFrustum(projMatrix * viewMatrix);
				//BoundingSphere modelBoundingSphere = skinnedMesh.SKinnedModel->mBoundingSphere;
				//modelBoundingSphere.Transform(transform);

				//if (CullBoundingSphere(frustum, modelBoundingSphere))
				//{
				//	BOREALIS_CORE_INFO("Culling entity {}", (int)entity);
				//	continue;
				//}

				//Renderer3D::SetLights(shader);
				materialShader->Bind();
				Renderer3D::SetLights(materialShader);
				Renderer3D::DrawSkinnedMesh(TransformComponent::GetGlobalTransform(brEntity), skinnedMesh, materialShader, (int)entity);
				materialShader->Unbind();
			}
		}

		
		Renderer3D::End();

		renderTarget->Unbind();
	}

	void Render2D::Execute(float dt)
	{
		if (shader) shader->Bind();

		Ref<RenderTargetSource> renderTarget = nullptr;
		bool editor = false;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				Renderer2D::Begin(std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj());
				editor = std::dynamic_pointer_cast<CameraSource>(sink->source)->editor;
			}

			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}
		}

		renderTarget->Bind();
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, SpriteRendererComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive() || (!editor && brEntity.HasComponent<CanvasRendererComponent>()))
				{
					continue;
				}

				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(TransformComponent::GetGlobalTransform(brEntity), sprite, (int)entity);
			}
		}
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(TransformComponent::GetGlobalTransform(brEntity), circle.Colour, circle.thickness, circle.fade, (int)entity);
			}
		}
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, TextComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, text] = group.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.text, text.font, TransformComponent::GetGlobalTransform(brEntity), (int)entity);
			}
		}
		

		Renderer2D::End();

		renderTarget->Unbind();
		if (shader) shader->Unbind();
	}

	GeometryPass::GeometryPass(std::string name) : EntityPass(name)
	{
		shader = s_shader;
		//shader = Shader::Create("../Borealis/engineResources/Shaders/Renderer3D_DeferredLighting.glsl");
	}

	void GeometryPass::Execute(float dt)
	{
		if (shader) shader->Bind();
		shader->Set("u_lightPass", false);
		Ref<FrameBuffer> gBuffer = nullptr;
		for (auto sink : sinkList)
		{
			if (sink->source)
			{
				auto sourcePtr = sink->source;

				if (sourcePtr->sourceType == RenderSourceType::GBuffer)
				{
					gBuffer = std::dynamic_pointer_cast<GBufferSource>(sourcePtr)->buffer;
					gBuffer->Bind();
					RenderCommand::Clear();
					RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
					//for (int i{1}; i < 4; i++)
					//{
					//	gBuffer->ClearAttachment(i, 0);
					//}
					gBuffer->ClearAttachment(1, -1);
				}

				if (sourcePtr->sourceType == RenderSourceType::Camera)
				{
					glm::mat4 viewProj = std::dynamic_pointer_cast<CameraSource>(sourcePtr)->GetViewProj();
					shader->Set("u_ViewProjection", viewProj);
				}
			}
		}

		{
			RenderCommand::DisableBlend();
			auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

				Renderer3D::DrawMesh(TransformComponent::GetGlobalTransform(brEntity), meshFilter, meshRenderer, shader,(int)entity);
			}
			RenderCommand::EnableBlend();
		}

		if (gBuffer) gBuffer->Unbind();
		if (shader) shader->Unbind();
	}

	LightingPass::LightingPass(std::string name) : EntityPass(name)
	{
		shader = s_shader;
	}

	void LightingPass::Execute(float dt)
	{
		if (shader) shader->Bind();
		shader->Set("u_lightPass", true);

		Ref<GBufferSource> gBuffer = nullptr;
		Ref<FrameBuffer> renderTarget = nullptr;
		for (auto sink : sinkList)
		{
			if (sink->source)
			{
				auto sourcePtr = sink->source;
				if (sourcePtr->sourceType == RenderSourceType::GBuffer)
				{
					gBuffer = std::dynamic_pointer_cast<GBufferSource>(sourcePtr);
				}

				if (sourcePtr->sourceType == RenderSourceType::RenderTargetColor)
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sourcePtr)->buffer;

					renderTarget->ClearAttachment(1, -1);
				}

				if (sourcePtr->sourceType == RenderSourceType::Camera)
				{
					glm::mat4 invViewProj = glm::inverse(std::dynamic_pointer_cast<CameraSource>(sourcePtr)->GetViewProj());
					shader->Set("u_invViewProj", invViewProj);
				}
			}
		}

		gBuffer->Bind();

		gBuffer->BindTexture(GBufferSource::Albedo,		0);
		gBuffer->BindTexture(GBufferSource::EntityID,	1);
		gBuffer->BindTexture(GBufferSource::Normal,		2);
		gBuffer->BindTexture(GBufferSource::Specular,	3);
		gBuffer->BindDepthBuffer(4);

		gBuffer->Unbind();
		
		shader->Set("lAlbedo",		0);
		shader->Set("lEntityID",	1);
		shader->Set("lNormal",		2);
		shader->Set("lSpecular",	3);
		shader->Set("lDepthBuffer",	4);

		{
			Renderer3D::End();//clear lights, move ltr on
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				lightComponent.position = TransformComponent::GetGlobalTranslate(brEntity);
				lightComponent.direction = TransformComponent::GetGlobalRotation(brEntity);
				Renderer3D::AddLight(lightComponent);
			}
			Renderer3D::SetLights(shader);
		}

		renderTarget->Bind();
		Renderer3D::DrawQuad();

		renderTarget->Unbind();

		shader->Unbind();
	}

	ShadowPass::ShadowPass(std::string name) : EntityPass(name)
	{
		shader = material_shader;
	}

	void ShadowPass::Execute(float dt)
	{
		shader->Bind();
		shader->Set("shadowPass", true);

		RenderCommand::EnableFrontFaceCull();

		glm::mat4 viewProjMatrix{};
		Ref<FrameBuffer> shadowMap = nullptr;
		glm::vec3 cameraPosition{};

		Ref<CameraSource> camera = nullptr;
		for (auto sink : sinkList)
		{
			if (sink->source)
			{
				auto sourcePtr = sink->source;

				if (sourcePtr->sourceType == RenderSourceType::RenderTargetColor)
				{
					shadowMap = std::dynamic_pointer_cast<RenderTargetSource>(sourcePtr)->buffer;
					shadowMap->Bind();
					RenderCommand::Clear();
				}

				if (sourcePtr->sourceType == RenderSourceType::Camera)
				{
					cameraPosition = std::dynamic_pointer_cast<CameraSource>(sourcePtr)->position;
					viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sourcePtr)->GetViewProj();

					camera = std::dynamic_pointer_cast<CameraSource>(sourcePtr);
				}
			}
		}

		{
			bool directionalLight = false;
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				if (!lightComponent.castShadow) continue;
				auto brEntity = Entity{ entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}
				lightComponent.position = TransformComponent::GetGlobalTranslate(brEntity);
				lightComponent.direction = TransformComponent::GetGlobalRotation(brEntity);

				if (lightComponent.type == LightComponent::Type::Directional)
				{
					directionalLight = true;
				}

				SetShadowVariable(lightComponent, shader, camera);
				break; //TODO 1 shadow for now
			}

			shadowMap->Bind();
			{
				auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
				for (auto& entity : group)
				{
					Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
					if (!brEntity.IsActive())
					{
						continue;
					}
					auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

					Renderer3D::DrawMesh(TransformComponent::GetGlobalTransform(brEntity), meshFilter, meshRenderer, shader, (int)entity);
				}
			}
			shadowMap->Unbind();

			if(directionalLight)
			{
				mCascadeShadowMapBuffer->Bind();
				{
					RenderCommand::Clear();
					auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
					for (auto& entity : group)
					{
						Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
						if (!brEntity.IsActive())
						{
							continue;
						}
						auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

						Renderer3D::DrawHighlightedMesh(TransformComponent::GetGlobalTransform(brEntity), meshFilter, cascade_shadow_shader);
					}
				}
				mCascadeShadowMapBuffer->Unbind();
			}
		}

		RenderCommand::EnableBackFaceCull();

		if (shadowMap)
			shadowMap->Unbind();

		shader->Unbind();
	}


	ObjectPickingPass::ObjectPickingPass(std::string name) : RenderPass(name)
	{
	}

	void ObjectPickingPass::Execute(float dt)
	{
		Ref<PixelBufferSource> pixelBuffer = nullptr;
		Ref<IntSource> entityID = nullptr;
		Ref<Vec2IntSource> mouse = nullptr;
		Ref<BoolSource> viewPortHovered = nullptr;
		Ref<RenderTargetSource> renderTarget = nullptr;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::PixelBuffer)
			{
				pixelBuffer = std::dynamic_pointer_cast<PixelBufferSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::IntRef)
			{
				entityID = std::dynamic_pointer_cast<IntSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::Vec2Int)
			{
				mouse = std::dynamic_pointer_cast<Vec2IntSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::Bool)
			{
				viewPortHovered = std::dynamic_pointer_cast<BoolSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}
		}

		if (pixelBuffer)
		{
			if (pixelBuffer->Width != renderTarget->Width || pixelBuffer->Height != renderTarget->Height)
			{
				pixelBuffer->Resize(renderTarget->Width, renderTarget->Height);
			}
			renderTarget->Bind();

			pixelBuffer->Bind();

			pixelBuffer->ReadTexture(1);

			pixelBuffer->Unbind();

			renderTarget->Unbind();
		}

		if(viewPortHovered->mRef)
		{
			if (SceneManager::GetActiveScene()->GetPixelBuffer()->ReadPixel(mouse->mRefX, mouse->mRefY) != -1)
			{
				//int id_ent = mViewportFrameBuffer->ReadPixel(1, mouseX, mouseY);
				entityID->mRef = SceneManager::GetActiveScene()->GetPixelBuffer()->ReadPixel(mouse->mRefX, mouse->mRefY);
				//BOREALIS_CORE_INFO("picking id {}", mHoveredEntity.GetName());
				//BOREALIS_CORE_INFO("Name : {}", mHoveredEntity.GetName());
			}
			else
			{
				entityID->mRef = -1;
			}
		}
	}

	EditorHighlightPass::EditorHighlightPass(std::string name) : RenderPass(name)
	{
		shader = common_shader;
	}

	void EditorHighlightPass::Execute(float dt)
	{
		Ref<RenderTargetSource> renderTarget = nullptr;
		glm::vec3 cameraLookAt{};

		glm::mat4 viewProjMatrix;
		Ref<IntListSource> selectedEntities = nullptr;
		int hoveredEntity = -1;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}

			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj();
				cameraLookAt = std::dynamic_pointer_cast<CameraSource>(sink->source)->lookAt;
			}

			if (sink->source->sourceType == RenderSourceType::IntList)
			{
				selectedEntities = std::dynamic_pointer_cast<IntListSource>(sink->source);
			}

			if (sink->source->sourceType == RenderSourceType::IntRef)
			{
				hoveredEntity = std::dynamic_pointer_cast<IntSource>(sink->source)->mRef;
			}
		}

		selectedEntities->mList.push_back(hoveredEntity);

		for(int entityID : selectedEntities->mList)
		{
			if (entityID != -1)
			{
				Entity brEntity = { (entt::entity)entityID, SceneManager::GetActiveScene().get() };

				if (!brEntity.IsActive())
				{
					return;
				}

				shader->Bind();
				shader->Set("u_ViewProjection", viewProjMatrix);
				if (entityID == hoveredEntity)
				{
					shader->Set("u_Filled", false);
				}
				else
				{
					shader->Set("u_Filled", true);
				}
				shader->Set("u_HighlightPass", false);
				shader->Set("u_Color", { 1.f, 0.475f, 0.f , 1.f });

				renderTarget->Bind();

				RenderCommand::ClearStencil();
				RenderCommand::EnableDepthTest();
				RenderCommand::ConfigureDepthFunc(DepthFunc::DepthLEqual);
				RenderCommand::EnableStencilTest();

				glm::mat4 transform = TransformComponent::GetGlobalTransform(brEntity);
				transform = glm::translate(transform, glm::normalize(cameraLookAt) * -0.5f);

				if (brEntity.HasComponent<SpriteRendererComponent>())
				{
					SpriteRendererComponent const& spriteRenderer = brEntity.GetComponent<SpriteRendererComponent>();
					Renderer2D::DrawHighlightedSprite(transform, spriteRenderer, shader);
				}
				if (brEntity.HasComponent<MeshFilterComponent>())
				{
					MeshFilterComponent const& meshFilter = brEntity.GetComponent<MeshFilterComponent>();
					Renderer3D::DrawHighlightedMesh(transform, meshFilter, shader);
				}
				if (brEntity.HasComponent<SkinnedMeshRendererComponent>())
				{

				}

				shader->Bind();
				shader->Set("u_Filled", false);
				shader->Set("u_HighlightPass", true);
				RenderCommand::EnableStencilTest();
				RenderCommand::EnableFrontFaceCull();
				RenderCommand::ConfigureStencilForHighlight();
				RenderCommand::EnableWireFrameMode();

				transform = glm::translate(transform, glm::normalize(cameraLookAt) * -0.01f);

				if (brEntity.HasComponent<SpriteRendererComponent>())
				{
					SpriteRendererComponent const& spriteRenderer = brEntity.GetComponent<SpriteRendererComponent>();
					Renderer2D::DrawHighlightedSprite(transform, spriteRenderer, shader);
				}
				if (brEntity.HasComponent<MeshFilterComponent>())
				{
					MeshFilterComponent const& meshFilter = brEntity.GetComponent<MeshFilterComponent>();
					Renderer3D::DrawHighlightedMesh(transform, meshFilter, shader);
				}
				if (brEntity.HasComponent<SkinnedMeshRendererComponent>())
				{

				}

				RenderCommand::DisableWireFrameMode();
				RenderCommand::EnableDepthTest();
				RenderCommand::ConfigureDepthFunc(DepthFunc::DepthLess);
				RenderCommand::EnableStencilTest();
				RenderCommand::EnableBackFaceCull();

				renderTarget->Unbind();

				shader->Unbind();
			}
		}
	}

	HighlightPass::HighlightPass(std::string name) : EntityPass(name)
	{
		shader = common_shader;
	}

	void HighlightPass::Execute(float dt)
	{
		Ref<RenderTargetSource> renderTarget = nullptr;
		glm::vec3 cameraLookAt{};

		glm::mat4 viewProjMatrix;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}

			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj();
				cameraLookAt = std::dynamic_pointer_cast<CameraSource>(sink->source)->lookAt;
			}
		}

		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, OutLineComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto [transformHolder, outline] = group.get<TransformComponent, OutLineComponent>(entity);

				if (!outline.active) continue;

				shader->Bind();
				shader->Set("u_ViewProjection", viewProjMatrix);
				shader->Set("u_Filled", outline.filled);
				shader->Set("u_HighlightPass", false);
				shader->Set("u_Color", outline.color);

				renderTarget->Bind();

				RenderCommand::ClearStencil();
				RenderCommand::EnableDepthTest();
				RenderCommand::ConfigureDepthFunc(DepthFunc::DepthLEqual);
				RenderCommand::EnableStencilTest();

				glm::mat4 transform = TransformComponent::GetGlobalTransform(brEntity);
				transform = glm::translate(transform, glm::normalize(cameraLookAt) * -0.01f);

				if (brEntity.HasComponent<SpriteRendererComponent>())
				{
					SpriteRendererComponent const& spriteRenderer = brEntity.GetComponent<SpriteRendererComponent>();
					Renderer2D::DrawHighlightedSprite(transform, spriteRenderer, shader);
				}
				if (brEntity.HasComponent<MeshFilterComponent>())
				{
					MeshFilterComponent const& meshFilter = brEntity.GetComponent<MeshFilterComponent>();
					Renderer3D::DrawHighlightedMesh(transform, meshFilter, shader);
				}
				if (brEntity.HasComponent<SkinnedMeshRendererComponent>())
				{

				}

				shader->Bind();
				shader->Set("u_Filled", false);
				shader->Set("u_HighlightPass", true);
				RenderCommand::EnableStencilTest();
				RenderCommand::EnableFrontFaceCull();
				RenderCommand::ConfigureStencilForHighlight();
				if(outline.lineWidth < 0.1f)
					RenderCommand::SetLineThickness(0.1f);
				else
					RenderCommand::SetLineThickness((outline.lineWidth > 10.f) ? 10.f : outline.lineWidth);
				RenderCommand::EnableWireFrameMode();

				transform = glm::translate(transform, glm::normalize(cameraLookAt) * -0.01f);

				if (brEntity.HasComponent<SpriteRendererComponent>())
				{
					SpriteRendererComponent const& spriteRenderer = brEntity.GetComponent<SpriteRendererComponent>();
					Renderer2D::DrawHighlightedSprite(transform, spriteRenderer, shader);
				}
				if (brEntity.HasComponent<MeshFilterComponent>())
				{
					MeshFilterComponent const& meshFilter = brEntity.GetComponent<MeshFilterComponent>();
					Renderer3D::DrawHighlightedMesh(transform, meshFilter, shader);
				}
				if (brEntity.HasComponent<SkinnedMeshRendererComponent>())
				{

				}

				RenderCommand::DisableWireFrameMode();
				RenderCommand::EnableDepthTest();
				RenderCommand::ConfigureDepthFunc(DepthFunc::DepthLess);
				RenderCommand::EnableStencilTest();
				RenderCommand::EnableBackFaceCull();

				renderTarget->Unbind();

				shader->Unbind();
			}
		}
	}

	Ref<FrameBuffer> UIFBO;

	UIPass::UIPass(std::string name) : EntityPass(name)
	{
		if (!UIFBO)
		{
			FrameBufferProperties props{ 1280, 720, false };
			props.Attachments = { FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
			UIFBO = FrameBuffer::Create(props);
		}

		shader = quad_shader;
	}

	void RenderCanvasRecursive(Entity parent, const glm::mat4& parentTransform, const glm::mat4& canvasTransform)
	{
		glm::mat4 globalTansform = (glm::mat4)parent.GetComponent<TransformComponent>();
		if (parent.HasComponent<CanvasRendererComponent>() && parent.HasComponent<SpriteRendererComponent>())
		{

			glm::mat4 transform = canvasTransform * parentTransform * globalTansform;

			Renderer2D::DrawSprite(transform, parent.GetComponent<SpriteRendererComponent>());
		}

		for (UUID childID : parent.GetComponent<TransformComponent>().ChildrenID)
		{
			Entity child = SceneManager::GetActiveScene()->GetEntityByUUID(childID);

			RenderCanvasRecursive(child, parentTransform * globalTansform, canvasTransform);
		}
	}

	void UIPass::Execute(float dt)
	{
		Ref<RenderTargetSource> renderTarget = nullptr;
		glm::mat4 viewProjMatrix{};

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}

			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj();
			}
		}

		if (UIFBO->GetProperties().Width != renderTarget->Width || UIFBO->GetProperties().Height != renderTarget->Height)
		{
			UIFBO->Resize(renderTarget->Width, renderTarget->Height);
		}

		const float baseScreenWidth = 1920.0f;
		const float baseScreenHeight = 1080.0f;
		viewProjMatrix = glm::ortho(0.0f, (float)renderTarget->Width, (float)renderTarget->Height, 0.0f, -100.0f, 100.0f);
		Renderer2D::Begin(viewProjMatrix);

		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, CanvasComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}
				auto [transform, canvas] = group.get<TransformComponent, CanvasComponent>(entity);

				float scaleFactor = 1 / canvas.scaleFactor * 0.95;

				glm::vec3 canvasPosition(renderTarget->Width * 0.5f, renderTarget->Height * 0.5f, 0.0f);
				glm::vec3 canvasScale(canvas.canvasSize.x * scaleFactor, canvas.canvasSize.y * scaleFactor, 1.0f);

				glm::mat4 canvasTransform = glm::translate(glm::mat4(1.0f), canvasPosition) *
					glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor * -1.f, 1.f));

				//render debug canvas
				{
					SpriteRendererComponent spriteRenderer;
					spriteRenderer.Colour = { 0.f,0.f,100.f,0.2f };

					glm::vec3 screenPosition(renderTarget->Width * 0.5f, renderTarget->Height * 0.5f, 0.0f);

					glm::vec3 scale = glm::vec3(
						canvas.canvasSize.x * scaleFactor,
						canvas.canvasSize.y * scaleFactor,
						1.f
					);
					glm::mat4 screenTransform = glm::translate(glm::mat4(1.0f), screenPosition) *
						glm::scale(glm::mat4(1.0f), scale);
					Renderer2D::DrawSprite(screenTransform, spriteRenderer);
				}

				RenderCanvasRecursive(brEntity, glm::mat4(1.0f), canvasTransform);

				//for (UUID childID : transform.ChildrenID)
				//{
				//	Entity child = SceneManager::GetActiveScene()->GetEntityByUUID(childID);

				//	if (!child.HasComponent<CanvasRendererComponent>()) continue;
				//	if (!child.HasComponent<SpriteRendererComponent>()) continue;

				//	glm::mat4 childTransform = child.GetComponent<TransformComponent>();
				//	//childTransform = glm::translate(childTransform, canvasPosition);
				//	//childTransform = glm::scale(childTransform, glm::vec3(scaleFactor));

				//	childTransform = canvasTransform * childTransform;

				//	Renderer2D::DrawSprite(childTransform, child.GetComponent<SpriteRendererComponent>());
				//}
			}
		}

		UIFBO->Bind();
		RenderCommand::Clear();

		RenderCommand::DisableDepthTest();
		Renderer2D::End();
		UIFBO->Unbind();

		//std::swap(UIFBO, renderTarget->buffer);

		RenderCommand::DisableDepthTest();
		UIFBO->BindTexture(0, 0);
		shader->Bind();
		shader->Set("u_Texture0", 0);

		renderTarget->Bind();
		Renderer3D::DrawQuad();
		renderTarget->Unbind();

		shader->Unbind();
		RenderCommand::EnableDepthTest();
	}

	EditorUIPass::EditorUIPass(std::string name) : EntityPass(name)
	{

	}

	void EditorUIPass::Execute(float dt)
	{
		Ref<RenderTargetSource> renderTarget = nullptr;
		Ref<RenderTargetSource> runTimeRenderTarget = nullptr;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				Renderer2D::Begin(std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj());
			}

			if (sink->source->sourceType == RenderSourceType::RenderTargetColor)
			{
				if (sink->sinkName == "renderTarget")
				{
					renderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
				if (sink->sinkName == "runTimeRenderTarget")
				{
					runTimeRenderTarget = std::dynamic_pointer_cast<RenderTargetSource>(sink->source);
				}
			}
		}

		renderTarget->Bind();
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, CanvasComponent>);
			for (auto& entity : group)
			{
				Entity brEntity = { entity, SceneManager::GetActiveScene().get() };
				if (!brEntity.IsActive())
				{
					continue;
				}

				auto [transform, canvas] = group.get<TransformComponent, CanvasComponent>(entity);
				canvas.scaleFactor = 0.01;
				canvas.canvasSize.x = runTimeRenderTarget->Width * canvas.scaleFactor;
				canvas.canvasSize.y = runTimeRenderTarget->Height * canvas.scaleFactor;
				SpriteRendererComponent sprite;
				sprite.Colour = { 0.f,0.f,100.f, 0.2f };
				glm::mat4 canvasTransform = glm::translate(glm::mat4(1.f), glm::vec3(((glm::mat4)transform)[3]));
				canvasTransform = glm::scale(canvasTransform, glm::vec3(canvas.canvasSize.x, canvas.canvasSize.y, 1.f));
				Renderer2D::DrawSprite(canvasTransform, sprite, (int)entity);
			}
		}
		Renderer2D::End();

		renderTarget->Unbind();
	}

	//========================================================================
	//RENDER GRAPH Config
	//========================================================================	

	RenderPassConfig::RenderPassConfig(RenderPassType type, std::string passName)
	{
		mType = type;
		mPassName = passName;

		if(!s_shader)
			s_shader = Shader::Create("engineResources/Shaders/Renderer3D_DeferredLighting.glsl");

		if (!material_shader)
			material_shader = Shader::Create("../Borealis/engineResources/Shaders/Renderer3D_Material.glsl");

		if (!cascade_shadow_shader)
			cascade_shadow_shader = Shader::Create("../Borealis/engineResources/Shaders/Renderer3D_CascadeShadow.glsl");

		if (!common_shader)
			common_shader = Shader::Create("../Borealis/engineResources/Shaders/Renderer3D_Common.glsl");

		if (!quad_shader)
			quad_shader = Shader::Create("../Borealis/engineResources/Shaders/Renderer3D_Quad.glsl");

		if (!mCascadeShadowMapBuffer)
		{
			FrameBufferProperties propsShadowMapBuffer{ 2024, 2024, false };
			propsShadowMapBuffer.Attachments = { FramebufferTextureFormat::DepthArray };
			mCascadeShadowMapBuffer = FrameBuffer::Create(propsShadowMapBuffer);
		}
	}

	RenderPassConfig& RenderPassConfig::AddSinkLinkage(std::string sinkName, std::string sourceName)
	{
		SinkLinkageInfo sinkLinkageInfo{ sinkName, sourceName };
		mSinkLinkageList.push_back(sinkLinkageInfo);

		return *this;
	}

	void RenderGraphConfig::AddPass(RenderPassConfig renderPassConfig)
	{
		passesConfigList.push_back(renderPassConfig);
	}

	void RenderGraphConfig::AddGlobalSource(Ref<RenderSource> globalSource)
	{
		globalRenderSourceList.push_back(globalSource);
	}

	//========================================================================
	//RENDER GRAPH
	//========================================================================	

	void RenderGraph::Init()
	{
		renderPassList.clear();
		globalSource.clear();
	}

	void RenderGraph::AddPass(Ref<RenderPass> pass)
	{
		renderPassList.push_back(pass);
	}

	void RenderGraph::Execute(float dt)
	{
		for (auto pass : renderPassList) 
		{
			bool skipPass = false;
			for (auto sink : pass->sinkList) 
			{
				if (!sink->source) 
				{
					//skiping pass rn, not good, do more error checks
					sink->source = FindSource(sink->sourceName);
					if (!sink->source)
					{
						skipPass = true;
						break;
					}
				}
			}
			if (skipPass) continue;
			pass->Execute(dt);
		}
	}

	void RenderGraph::SetConfig(RenderGraphConfig renderGraphConfig)
	{
		mRenderGraphConfig = renderGraphConfig;
	}

	void RenderGraph::Finalize()
	{
		for (auto const& globalSource : mRenderGraphConfig.globalRenderSourceList)
		{
			SetGlobalSource(globalSource);
		}

		for (auto const& passesConfig : mRenderGraphConfig.passesConfigList)
		{
			switch (passesConfig.mType)
			{
			case RenderPassType::Render3D:
			case RenderPassType::Render2D:
			case RenderPassType::Geometry:
			case RenderPassType::Lighting:
			case RenderPassType::Shadow:
			case RenderPassType::HighlightPass:
			case RenderPassType::UIPass:
			case RenderPassType::EditorUIPass:
				AddEntityPassConfig(passesConfig);
				break;
			case RenderPassType::ObjectPicking:
			case RenderPassType::EditorHighlightPass:
				AddRenderPassConfig(passesConfig);
				break;
			default:
				break;
			}
		}
	}

	Ref<RenderSource> RenderGraph::FindSource(std::string sourceName)
	{
		for (auto global : globalSource) 
		{
			if (global->sourceName == sourceName) 
			{
				return global;
			}
		}

		for (auto pass : renderPassList) 
		{
			for (auto sink : pass->sinkList) 
			{
				if (sink->outputName == sourceName) 
				{
					return sink->source;
				}
			}
		}

		return nullptr;
	}

	void RenderGraph::AddRenderPassConfig(RenderPassConfig const& renderPassConfig)
	{
		Ref<RenderPass> renderPass = nullptr;
		switch (renderPassConfig.mType)
		{
		case RenderPassType::ObjectPicking:
			renderPass = MakeRef<ObjectPickingPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::EditorHighlightPass:
			renderPass = MakeRef<EditorHighlightPass>(renderPassConfig.mPassName);
			break;
		}

		for (auto const& sinkLinkage : renderPassConfig.mSinkLinkageList)
		{
			renderPass->SetSinkLinkage(sinkLinkage.sinkName, sinkLinkage.sourceName);
		}

		AddPass(renderPass);
	}

	void RenderGraph::AddEntityPassConfig(RenderPassConfig const& renderPassConfig)
	{
		Ref<RenderPass> renderPass = nullptr;
		switch (renderPassConfig.mType)
		{
		case RenderPassType::Render3D:
			renderPass = MakeRef<Render3D>(renderPassConfig.mPassName);
			break;
		case RenderPassType::Render2D:
			renderPass = MakeRef<Render2D>(renderPassConfig.mPassName);
			break;
		case RenderPassType::Geometry:
			renderPass = MakeRef<GeometryPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::Lighting:
			renderPass = MakeRef<LightingPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::Shadow:
			renderPass = MakeRef<ShadowPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::HighlightPass:
			renderPass = MakeRef<HighlightPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::UIPass:
			renderPass = MakeRef<UIPass>(renderPassConfig.mPassName);
			break;
		case RenderPassType::EditorUIPass:
			renderPass = MakeRef<EditorUIPass>(renderPassConfig.mPassName);
			break;
		}

		Ref<EntityPass> entityPass = std::dynamic_pointer_cast<EntityPass>(renderPass);
		entityPass->SetEntityRegistry(*registryPtr);

		for (auto const& sinkLinkage : renderPassConfig.mSinkLinkageList)
		{
			renderPass->SetSinkLinkage(sinkLinkage.sinkName, sinkLinkage.sourceName);
		}

		AddPass(renderPass);
	}

	void RenderGraph::SetGlobalSource(Ref<RenderSource> source)
	{
		globalSource.push_back(source);
	}

	void RenderGraph::SetEntityRegistry(entt::registry& registry)
	{
		registryPtr = &registry;
	}

	void RenderGraph::SetFinalSink(std::string sinkName, std::string sourceName)
	{

	}
}

