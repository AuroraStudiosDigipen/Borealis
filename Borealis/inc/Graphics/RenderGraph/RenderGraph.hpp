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
#include <Graphics/EditorCamera.hpp>
#include <Graphics/Framebuffer.hpp>
#include <Graphics/Shader.hpp>

#include <string>
#include <vector>

#include <entt.hpp>

namespace Borealis
{
	enum class RenderSourceType
	{
		RenderTargetColor,
		GBuffer,
		Texture2D,
		UniformBuffers,
		Camera
	};

	class RenderSource
	{
	public:
		std::string sourceName;
		RenderSourceType sourceType;
		virtual void Bind() = 0;
		virtual void Unbind() {};
	};

	class RenderTargetSource : public RenderSource
	{
	public:
		RenderTargetSource(std::string name, Ref<FrameBuffer> framebuffer);
		void Bind() override;
		void Unbind() override;
		Ref<FrameBuffer> buffer;
	};

	class GBufferSource : public RenderSource
	{
	public:
		enum TextureType : uint32_t
		{
			Albedo = 0,
			EntityID,
			Normal,
			Specular,
			Position,
			Metallic,
			Roughness
		};
		GBufferSource(std::string name, Ref<FrameBuffer> framebuffer);

		void Bind() override;

		void Unbind() override;

		void BindTexture(TextureType type, int index);

		Ref<FrameBuffer> buffer;
	};

	class CameraSource : public RenderSource
	{
	public:
		CameraSource(std::string name, EditorCamera const& camera);
		CameraSource(std::string name, const Camera& camera, const glm::mat4& transform);

		bool editor;
		glm::mat4 viewProj;
		
		void Bind() override {};
		glm::mat4 GetViewProj();
	};

	class RenderSink
	{
	public:
		std::string sinkName;
		std::string outputName;//source belonging to the sink, pass+sinkName;

		std::string sourceName{};
		Ref<RenderSource> source = nullptr;
	};

	enum class RenderPassType
	{
		Render3D,
		Render2D,
		Geometry,
		Lighting
	};

	class RenderPass 
	{
	public:
		RenderPass(std::string name);
		std::string passName;

		void SetSinkLinkage(std::string sinkName, std::string sourceName);
		virtual void Execute() = 0;

		void Bind();
		void Unbind();

		std::vector<Ref<RenderSink>> sinkList;
		std::vector<Ref<RenderSource>> sourceList;
		Ref<Shader> shader;
	};

	class EntityPass : public RenderPass
	{
	public:
		EntityPass(std::string name) : RenderPass(name), registryPtr(nullptr) {};
		void SetEntityRegistry(entt::registry& registry);
		void Execute() override {};

		entt::registry* registryPtr;
	};

	class Render3D : public EntityPass
	{
	public:
		Render3D(std::string name) : EntityPass(name) {};
		void Execute() override;
	};

	class Render2D : public EntityPass
	{
	public:
		Render2D(std::string name) : EntityPass(name) {};
		void Execute() override;
	};

	class GeometryPass : public EntityPass
	{
	public:
		GeometryPass(std::string name);

		void Execute() override;
	};

	class LightingPass : public EntityPass
	{
	public:
		LightingPass(std::string name);

		void Execute() override;
	};

	struct RenderPassConfig
	{
		RenderPassType mType;
		std::string mPassName;

		struct SinkLinkageInfo
		{
			std::string sinkName;
			std::string sourceName;
		};

		std::vector<SinkLinkageInfo> mSinkLinkageList;

		RenderPassConfig(RenderPassType type, std::string passName);
		void AddSinkLinkage(std::string sinkName, std::string sourceName);
	};

	class RenderGraphConfig
	{
	public:
		void AddPass(RenderPassConfig renderPassConfig);
		void AddGlobalSource(Ref<RenderSource> globalSource);

		std::vector<Ref<RenderSource>> globalRenderSourceList;
		std::vector<RenderPassConfig> passesConfigList;
	};

	class RenderGraph
	{
	public:
		void Init();
		void AddPass(Ref<RenderPass> pass);
		void Execute();

		void SetConfig(RenderGraphConfig renderGraphConfig);

		void Finalize();

		Ref<RenderSource> FindSource(std::string sourceName);

		void AddEntityPassConfig(RenderPassConfig const& renderPassConfig);

		void SetGlobalSource(Ref<RenderSource> source);

		void SetEntityRegistry(entt::registry& registry);
		void SetFinalSink(std::string sinkName, std::string sourceName);//set render output

		std::vector<Ref<RenderPass>> renderPassList;
		std::vector<Ref<RenderSource>> globalSource;

		RenderGraphConfig mRenderGraphConfig;

		entt::registry* registryPtr;
	};
}

#endif
