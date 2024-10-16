///******************************************************************************
///*!
//\file       RenderGraph.cpp
//\author     Chan Guo Geng Gordon
//\par        email: g.chan/@digipen.edu
//\date       September 07, 2024
//\brief      Defines
//
//Copyright (C) 2024 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the
//prior written consent of DigiPen Institute of Technology is prohibited.
// */
// /******************************************************************************/
//
#include <BorealisPCH.hpp>
//
//#include <Graphics/RenderGraph/RenderGraphOld.hpp>
//
//#include <Graphics/Renderer3D.hpp>
//#include <Scene/Components.hpp>
//
//namespace Borealis
//{
//	void RenderPass3D::Execute()
//	{
//		for (auto sink : sinkList) 
//		{
//			if (sink->source) 
//			{
//				auto source = sink->source;
//				source->Bind(); // bind frame buffers, uniforms, textures and stuff
//			}
//		}
//
//		if (mRegistry == nullptr) return;
//
//		{
//			auto group = mRegistry->group<>(entt::get<TransformComponent, LightComponent>);
//			for (auto& entity : group)
//			{
//				auto [transform, lightComponent] = group.get<TransformComponent, LightComponent>(entity);
//				lightComponent.offset = transform.Translate;
//				Renderer3D::AddLight(lightComponent);
//			}
//		}
//		{
//			auto group = mRegistry->group<>(entt::get<TransformComponent, MeshFilterComponent, MeshRendererComponent>);
//			for (auto& entity : group)
//			{
//				auto [transform, meshFilter, meshRenderer] = group.get<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entity);
//				
//				Renderer3D::DrawMesh(transform, meshFilter, meshRenderer, (int)entity);
//			}
//		}
//
//		for (auto sink : sinkList)
//		{
//			if (sink->source)
//			{
//				auto source = sink->source;
//				source->Unbind(); // bind frame buffers, uniforms, textures and stuff
//			}
//		}
//	}
//
//	void RenderGraph::AddGlobalSource(std::shared_ptr<RenderSource> source)
//	{
//		globalSource.push_back(source);
//	}
//
//	void RenderGraph::AddRenderPass(std::shared_ptr<RenderPass> pass)
//	{
//		passList.push_back(pass);
//	}
//
//	void RenderGraph::Execute()
//	{
//		for (auto pass : passList)
//		{
//			pass->Execute();
//		}
//	}
//
//	BufferSource::BufferSource(std::string name, Ref<FrameBuffer> framebuffer)
//	{
//		mName = name;
//		frameBuffer = framebuffer;
//	}
//
//	void BufferSource::Bind()
//	{
//		if (frameBuffer)
//			frameBuffer->Bind();
//	}
//	void BufferSource::Unbind()
//	{
//		if (frameBuffer)
//			frameBuffer->Unbind();
//	}
//	void EntityPass::LinkEntityRegistry(entt::registry& registry)
//	{
//		mRegistry = &registry;
//	}
//
//	void RenderPass::AddSource(std::shared_ptr<RenderSource> source)
//	{
//		sourceList.push_back(source);
//	}
//
//	void RenderPass::AddSink(std::shared_ptr<RenderSink> sink)
//	{
//		sinkList.push_back(sink);
//	}
//}
//
