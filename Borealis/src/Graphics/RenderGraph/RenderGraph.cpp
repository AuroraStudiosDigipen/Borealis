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

#include <Assets/AssetManager.hpp>
#include <Graphics/RenderGraph/RenderGraph.hpp>
#include <Graphics/Renderer3D.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Scene/Components.hpp>

namespace Borealis
{
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

	CameraSource::CameraSource(std::string name, EditorCamera const& camera)
	{
		sourceName = name;
		sourceType = RenderSourceType::Camera;

		viewProj = camera.GetViewProjectionMatrix();
		editor = true;
	}

	CameraSource::CameraSource(std::string name, const Camera& camera, const glm::mat4& transform)
	{
		sourceName = name;
		sourceType = RenderSourceType::Camera;

		viewProj = camera.GetProjectionMatrix() * glm::inverse(transform);
		editor = false;
	}

	glm::mat4 CameraSource::GetViewProj()
	{
		return viewProj;
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
	void Render3D::Execute()
	{
		for (auto sink : sinkList)
		{
			if (sink->source->sourceType == RenderSourceType::Camera)
			{
				Renderer3D::Begin(std::dynamic_pointer_cast<CameraSource>(sink->source)->GetViewProj());
				break;
			}
		}

		{
			entt::basic_group group = registryPtr->group<>(entt::get<TransformComponent, LightComponent>);
			for (auto& entity : group)
			{
				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
				lightComponent.offset = transform.Translate;
				Renderer3D::AddLight(lightComponent);
			}
		}
		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);
				
				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, (int)entity);
			}
		}
		Renderer3D::End();
	}

	void Render2D::Execute()
	{
		for (auto source : sourceList)
		{
			if (source->sourceType == RenderSourceType::Camera)
			{
				Renderer2D::Begin(std::dynamic_pointer_cast<CameraSource>(source)->GetViewProj());
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
	}

	GeometryPass::GeometryPass(std::string name) : EntityPass(name)
	{
		shader = Shader::Create("engineResources/Shaders/Renderer3D_DeferredLighting.glsl");
	}

	void GeometryPass::Execute()
	{
		shader->Set("lightPass", false);

		{
			auto group = registryPtr->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
			for (auto& entity : group)
			{
				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);

				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, (int)entity);
			}
		}
		Renderer3D::End();
	}

	LightingPass::LightingPass(std::string name) : RenderPass(name)
	{
		//duplicate shader, move to assets manager
		shader = Shader::Create("engineResources/Shaders/Renderer3D_DeferredLighting.glsl");
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

	void RenderGraph::Execute()
	{
		for (auto pass : renderPassList) 
		{
			for (auto sink : pass->sinkList) 
			{
				if (!sink->source) 
				{
					sink->source = FindSource(sink->sourceName);
				}
			}

			pass->Bind();
			pass->Execute();
			pass->Unbind();
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
	}

	void RenderGraph::SetGlobalSource(Ref<RenderSource> source)
	{
		globalSource.push_back(source);
	}

	void RenderGraph::SetFinalSink(std::string sinkName, std::string sourceName)
	{

	}
}

