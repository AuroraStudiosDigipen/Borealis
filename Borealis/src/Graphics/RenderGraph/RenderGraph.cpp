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

namespace Borealis
{
	Ref<Shader> s_shader = nullptr;
	Ref<Shader> shadow_shader = nullptr;

	Ref<SkinnedModel> skinnedModel = nullptr;
	Ref<Shader> skinnedShader = nullptr;
	Animator animator(nullptr);

	//========================================================================
	//BUFFER SOURCE
	//========================================================================
	RenderTargetSource::RenderTargetSource(std::string name, Ref<FrameBuffer> framebuffer)
	{
		sourceName = name;
		sourceType = RenderSourceType::RenderTargetColor;

		buffer = framebuffer;
	}

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

	void RenderTargetSource::BindDepthBuffer(int index)
	{
		buffer->BindDepthBuffer(index);
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

	CameraSource::CameraSource(std::string name, EditorCamera const& camera)
	{
		sourceName = name;
		sourceType = RenderSourceType::Camera;

		projMtx = camera.GetProjectionMatrix();
		viewMtx = camera.GetViewMatrix();
		viewProj = camera.GetViewProjectionMatrix();
		viewPortSize = camera.GetViewPortSize();
		position = camera.GetPosition();
		editor = true;
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

		editor = false;
	}

	glm::mat4 CameraSource::GetViewProj()
	{
		return viewProj;
	}

	glm::vec2 CameraSource::GetViewPortSize()
	{
		return viewPortSize;
	}

	//TextureSource::TextureSource(std::string name)
	//{
	//	sourceName = name;
	//	sourceType = RenderSourceType::Texture2D;
	//}

	//void TextureSource::AddTexture(AssetHandle textureHandle)
	//{
	//	textureAssetHandles.push_back(textureHandle);
	//	textureList.push_back(AssetManager::GetAsset<Texture2D>(textureHandle));
	//}

	//void TextureSource::Bind()
	//{
	//	for (int i{}; i < textureList.size(); i++)
	//	{
	//		textureList[i]->Bind(i);
	//	}
	//}

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

	void SetShadowAndLight(Ref<RenderTargetSource> renderTarget, Ref<RenderTargetSource> shadowMap, Ref<Shader> shader,  entt::registry* registryPtr)
	{
		if (shadowMap)
		{
			shadowMap->BindDepthBuffer(0);
			shader->Set("u_ShadowMap", 0);
		}

		shader->Set("shadowPass", false);
		renderTarget->Bind();
		//add light to light engine and shadow pass
		{
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				lightComponent.offset = transform.Translate;
				Renderer3D::AddLight(lightComponent);


				if (lightComponent.type == LightComponent::Type::Spot)
				{
					glm::vec3 upVector = { 0.f,1.f,0.f };
					glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.offset + lightComponent.spotLightDirection, upVector);
					float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x * 2.f); // Spotlight cone angle
					glm::mat4 lightProj = glm::perspective(fieldOfView, 1.f, 20.f, 1000.f);

					shader->Set("u_LightViewProjection", lightProj * lightView);
				}
				else if (lightComponent.type == LightComponent::Type::Directional)
				{
					glm::vec3 upVector = { 0.f,1.f,0.f };
					glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.direction, upVector);
					float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x * 2.f); // Spotlight cone angle
					glm::mat4 lightProj = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.f, 400.f);

					shader->Set("u_LightViewProjection", lightProj * lightView);
				}
			}
		}
	}
	
	void Render3D::Execute(float dt)
	{
		glm::mat4 viewProjMatrix{};
		bool editor{};

		Ref<RenderTargetSource> renderTarget = nullptr;
		Ref<RenderTargetSource> shadowMap = nullptr;

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				viewProjMatrix = std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj();
				editor = std::dynamic_pointer_cast<CameraSource>(sink->source)->editor;
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
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

				if (!meshFilter.Model || !meshRenderer.Material) continue;

				Frustum frustum = ComputeFrustum(viewProjMatrix);
				BoundingSphere modelBoundingSphere = meshFilter.Model->mBoundingSphere;

				modelBoundingSphere.Transform(transform);

				if (CullBoundingSphere(frustum, modelBoundingSphere))
				{
					BOREALIS_CORE_INFO("Culling entity {}", (int)entity);
					continue;
				}

				Ref<Shader> materialShader = meshRenderer.Material->GetShader();

				Renderer3D::Begin(viewProjMatrix, materialShader);

				SetShadowAndLight(renderTarget, shadowMap, materialShader, registryPtr);

				Renderer3D::SetLights(materialShader);
				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, materialShader, (int)entity);
			}
		}
		//skinned mesh pass
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, SkinnedMeshRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, skinnedMesh] = group.get<TransformComponent, SkinnedMeshRendererComponent>(entity);

				if (!skinnedMesh.SkinnnedModel || !skinnedMesh.Material) continue;

				Ref<Shader> materialShader = skinnedMesh.Material->GetShader();

				Renderer3D::Begin(viewProjMatrix, materialShader);

				materialShader->Bind();
				materialShader->Set("u_HasAnimation", false);

				SetShadowAndLight(renderTarget, shadowMap, materialShader, registryPtr);

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
				Renderer3D::DrawSkinnedMesh(transform, skinnedMesh, materialShader, (int)entity);
				materialShader->Unbind();
			}
		}



		//if (shadowMap)
		//{
		//	shadowMap->BindDepthBuffer(0);
		//	shader->Set("u_ShadowMap", 0);
		//}

		//shader->Set("shadowPass", false);
		//renderTarget->Bind();
		////add light to light engine and shadow pass
		//{
		//	entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
		//	for (auto& entity : group)
		//	{
		//		auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
		//		lightComponent.offset = transform.Translate;
		//		Renderer3D::AddLight(lightComponent);


		//		if (lightComponent.type == LightComponent::Type::Spot)
		//		{
		//			glm::vec3 upVector = { 0.f,1.f,0.f };
		//			glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.offset + lightComponent.spotLightDirection, upVector);
		//			float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x * 2.f); // Spotlight cone angle
		//			glm::mat4 lightProj = glm::perspective(fieldOfView, 1.f, 20.f, 1000.f);

		//			shader->Set("u_LightViewProjection", lightProj * lightView);
		//		}
		//		else if (lightComponent.type == LightComponent::Type::Directional)
		//		{
		//			glm::vec3 upVector = { 0.f,1.f,0.f };
		//			glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.direction, upVector);
		//			float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x * 2.f); // Spotlight cone angle
		//			glm::mat4 lightProj = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.f, 400.f);

		//			shader->Set("u_LightViewProjection", lightProj * lightView);
		//		}
		//	}
		//}
		
		Renderer3D::End();

		renderTarget->Unbind();
	}

	void Render2D::Execute(float dt)
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

		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				Renderer2D::Begin(std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj());
				break;
			}
		}

		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, SpriteRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}
		}
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform, circle.Colour, circle.thickness, circle.fade, (int)entity);
			}
		}
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, TextComponent>);
			for (auto& entity : group)
			{
				auto [transform, text] = group.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.text, text.font, transform, (int)entity);
			}
		}
		Renderer2D::End();

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
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, shader,(int)entity);
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
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				lightComponent.offset = transform.Translate;
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
		shader = shadow_shader;
	}

	void ShadowPass::Execute(float dt)
	{
		shader->Bind();
		shader->Set("shadowPass", true);

		glm::mat4 viewProjMatrix;
		Ref<FrameBuffer> shadowMap = nullptr;
		glm::vec3 cameraPosition{};
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
				}
			}
		}

		{
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				if (!lightComponent.castShadow) continue;

				if (lightComponent.type == LightComponent::Type::Spot)
				{
					glm::vec3 upVector = { 0.f,1.f,0.f };
					glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.offset + lightComponent.spotLightDirection, upVector);
					float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x*2.f); // Spotlight cone angle
					glm::mat4 lightProj = glm::perspective(fieldOfView, 1.f, 0.3f, 1000.f);

					shader->Set("u_LightViewProjection", lightProj * lightView);
				}
				else if (lightComponent.type == LightComponent::Type::Directional)
				{
					glm::vec3 upVector = { 0.f,1.f,0.f };
					glm::vec3 lightPos = cameraPosition + lightComponent.offset;
					glm::mat4 lightView = glm::lookAt(lightComponent.offset, lightComponent.direction, upVector);
					float fieldOfView = glm::radians(lightComponent.InnerOuterSpot.x * 2.f); // Spotlight cone angle
					glm::mat4 lightProj = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.f, 400.f);

					shader->Set("u_LightViewProjection", lightProj * lightView);
				}
			}

			{
				auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
				for (auto& entity : group)
				{
					auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

					Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, shader, (int)entity);
				}
			}
		}

		if (shadowMap)
			shadowMap->Unbind();

		shader->Unbind();
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

		if (!shadow_shader)
			shadow_shader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");
	}

	void RenderPassConfig::AddSinkLinkage(std::string sinkName, std::string sourceName)
	{
		SinkLinkageInfo sinkLinkageInfo{ sinkName, sourceName };
		mSinkLinkageList.push_back(sinkLinkageInfo);
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
				AddEntityPassConfig(passesConfig);
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

