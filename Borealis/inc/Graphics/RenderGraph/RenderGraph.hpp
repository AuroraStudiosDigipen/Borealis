/******************************************************************************
/*!
\file       RenderGraph.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 16, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RenderGraph_HPP
#define RenderGraph_HPP

#include <Core/Core.hpp>
#include <Graphics/Framebuffer.hpp>

#include <string>
#include <vector>

namespace Borealis
{
	enum class RenderSourceType
	{
		RenderTargetColor,
		Texture2D,
		UniformBuffers
	};

	class RenderSource
	{
	public:
		std::string sourceName;
		RenderSourceType sourceType;
		virtual void Bind() = 0;
	};

	class BufferSource : public RenderSource
	{
	public:
		BufferSource(std::string name, Ref<FrameBuffer> framebuffer);
		void Bind() override;
		Ref<FrameBuffer> buffer;
	};

	class RenderSink
	{
	public:
		std::string sinkName;
		std::string outputName;//source belonging to the sink, pass+sinkName;

		Ref<RenderSource> outputSource;
	};

	class RenderPass 
	{
	public:
		std::string passName;

		void SetSinkLinkage(std::string sinkName, std::string sourceName);
		virtual void Execute() = 0;

		std::vector<Ref<RenderSink>> sinkList;
	};

	class EntityPass : public RenderPass
	{
	public:
		void SetEntityRegistry();
		void Execute() override {};

		//entt registry
	};

	class RenderGraph
	{
	public:
		void AddPass(Ref<RenderPass> pass);
		void Execute();

		void SetGlobalSource(Ref<RenderSource> source);
		void SetFinalSink();//set render output

		std::vector<Ref<RenderPass>> renderPassList;
		std::vector<Ref<RenderSource>> globalSource;
	};
}

#endif
