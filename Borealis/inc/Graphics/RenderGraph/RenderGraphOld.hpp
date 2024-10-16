///******************************************************************************
///*!
//\file       RenderGraph.hpp
//\author     Chan Guo Geng Gordon
//\par        email: g.chan/@digipen.edu
//\date       October 16, 2024
//\brief      Declares
//
//Copyright (C) 2024 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the
//prior written consent of DigiPen Institute of Technology is prohibited.
// */
// /******************************************************************************/
//
//#ifndef RenderGraph_HPP
//#define RenderGraph_HPP
//
//#include <string>
//#include <memory>
//#include <vector>
//
//#include <Graphics/Framebuffer.hpp>
//
//#include <entt.hpp>
//
//namespace Borealis
//{
//	enum class RenderSourceType
//	{
//		RenderTargetColor,
//		Texture2D,
//		UniformBuffer
//	};
//
//	enum class RenderSourceOrigin
//	{
//		Global,
//		Other,
//		Self
//	};
//
//	struct RenderSource
//	{
//		std::string			mName;
//		RenderSourceType	mType;
//		RenderSourceOrigin	mOrigin;
//
//		virtual void Bind() = 0;
//		virtual void Unbind() = 0;
//	};
//
//	//for framebuffers
//	struct BufferSource : public RenderSource
//	{
//		BufferSource(std::string name, Ref<FrameBuffer> framebuffer);
//		void Bind() override;
//		void Unbind() override;
//
//		Ref<FrameBuffer> frameBuffer;
//	};
//
//	struct RenderSink
//	{
//		std::string		name;
//		std::shared_ptr<RenderSource> source;
//	};
//
//	class RenderPass
//	{
//	public:
//		void Init();
//		virtual void Execute() = 0;
//
//		void AddSource(std::shared_ptr<RenderSource> source);
//		void AddSink(std::shared_ptr<RenderSink> sink);
//
//		std::string name;
//		std::vector<std::shared_ptr<RenderSource>> sourceList;
//		std::vector<std::shared_ptr<RenderSink>> sinkList;
//	};
//
//	class EntityPass : public RenderPass
//	{
//	public:
//		void LinkEntityRegistry(entt::registry& registry);
//		entt::registry* mRegistry = nullptr;
//	};
//
//	class RenderPass3D : public EntityPass
//	{
//	public:
//		void Execute() override;
//	};
//
//	class RenderGraph
//	{
//	public:
//		void AddGlobalSource(std::shared_ptr<RenderSource> source);
//		void AddRenderPass(std::shared_ptr<RenderPass> pass);
//		void Execute();
//	private:
//		std::vector<std::shared_ptr<RenderSource>> globalSource;
//		std::vector<std::shared_ptr<RenderPass>> passList;
//		RenderSink output;
//	};
//}
//
//#endif
