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
#include <Graphics/Pixelbuffer.hpp>
#include <Graphics/Shader.hpp>

#include <string>
#include <vector>

#include <entt.hpp>

namespace Borealis
{
	enum class RenderSourceType
	{
		Bool,
		IntRef,
		Vec2Int,
		IntList,
		RenderTargetColor,
		GBuffer,
		PixelBuffer,
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

	class BoolSource : public RenderSource
	{
	public:
		BoolSource(std::string name, bool ref);

		void Bind() override;
		void Unbind() override;

		bool mRef;
	};

	class IntSource : public RenderSource
	{
	public:
		IntSource(std::string name, int& ref);

		void Bind() override;
		void Unbind() override;

		int& mRef;
	};

	class Vec2IntSource : public RenderSource
	{
	public:
		Vec2IntSource(std::string name, int refX, int refY);

		void Bind() override;
		void Unbind() override;

		int mRefX;
		int mRefY;
	};

	class IntListSource : public RenderSource
	{
	public:
		IntListSource(std::string name, std::list<int> const& intList);
		void Bind() override;
		void Unbind() override;

		std::list<int> mList;
	};

	class RenderTargetSource : public RenderSource
	{
	public:
		RenderTargetSource(std::string name, Ref<FrameBuffer> framebuffer);
		void Bind() override;
		void Unbind() override;

		void BindDepthBuffer(int index, bool is3D = false);

		Ref<FrameBuffer> buffer;
		uint32_t Width, Height;

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

		void BindDepthBuffer(int index);

		Ref<FrameBuffer> buffer;
	};

	class PixelBufferSource : public RenderSource
	{
	public:
		PixelBufferSource(std::string name, Ref<PixelBuffer> pixelbuffer);
		void ReadTexture(uint32_t index);
		void Bind() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height);
		Ref<PixelBuffer> buffer;
		uint32_t Width, Height;
	};

	class CameraSource : public RenderSource
	{
	public:
		CameraSource(std::string name, EditorCamera const& camera);
		CameraSource(std::string name, const Camera& camera, const glm::mat4& transform);

		bool editor;
		glm::vec3 position;
		glm::vec3 lookAt;
		glm::mat4 projMtx;
		glm::mat4 viewMtx;
		glm::mat4 viewProj;
		glm::vec2 viewPortSize;
		float fov;
		float nearPlane;
		float farPlane;
		float aspectRatio;
		
		void Bind() override {};
		glm::mat4 GetViewProj();
		glm::vec2 GetViewPortSize();
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
		Lighting,
		Shadow,
		ObjectPicking,
		HighlightPass,
		EditorHighlightPass,
		UIPass,
		EditorUIPass,
		ParticleSystemPass,
		SkyboxPass,
		RenderToTarget,
		UIWorldPass
	};

	class RenderPass 
	{
	public:
		RenderPass(std::string name);
		std::string passName;

		void SetSinkLinkage(std::string sinkName, std::string sourceName);
		virtual void Execute(float dt) = 0;

		void Bind();
		void Unbind();

		std::vector<Ref<RenderSink>> sinkList;
		std::vector<Ref<RenderSource>> sourceList;
		Ref<Shader> shader;
	};

	class ObjectPickingPass : public RenderPass
	{
	public:
		ObjectPickingPass(std::string name);

		void Execute(float dt) override;
	};

	class EditorHighlightPass : public RenderPass
	{
	public:
		EditorHighlightPass(std::string name);

		void Execute(float dt) override;
	};


	class SkyboxPass : public RenderPass
	{
	public:
		SkyboxPass(std::string name);

		void Execute(float dt) override;
	};

	class RenderToTarget : public RenderPass
	{
	public:
		RenderToTarget(std::string name);

		void Execute(float dt) override;
	};

	class EntityPass : public RenderPass
	{
	public:
		EntityPass(std::string name) : RenderPass(name), registryPtr(nullptr) {};
		void SetEntityRegistry(entt::registry& registry);
		void Execute(float dt) override {};

		entt::registry* registryPtr;
	};

	class Render3D : public EntityPass //should use shader from material
	{
	public:
		Render3D(std::string name);
		void Execute(float dt) override;
	};

	class Render2D : public EntityPass
	{
	public:
		Render2D(std::string name) : EntityPass(name) {};
		void Execute(float dt) override;
	};

	class GeometryPass : public EntityPass //should use shader from material
	{
	public:
		GeometryPass(std::string name);

		void Execute(float dt) override;
	};

	class LightingPass : public EntityPass
	{
	public:
		LightingPass(std::string name);

		void Execute(float dt) override;
	};

	class ShadowPass : public EntityPass
	{
	public:
		ShadowPass(std::string name);

		void Execute(float dt) override;
	};

	class HighlightPass : public EntityPass
	{
	public:
		HighlightPass(std::string name);

		void Execute(float dt) override;
	};

	class UIPass : public EntityPass
	{
	public:
		UIPass(std::string name);

		void Execute(float dt) override;
	};

	class UIWorldPass : public EntityPass
	{
	public:
		UIWorldPass(std::string name);

		void Execute(float dt) override;
	};

	class EditorUIPass : public EntityPass
	{
	public:
		EditorUIPass(std::string name);

		void Execute(float dt) override;
	};

	class ParticleSystemPass : public EntityPass
	{
	public:
		ParticleSystemPass(std::string name);

		void Execute(float dt) override;
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
		RenderPassConfig& AddSinkLinkage(std::string sinkName, std::string sourceName);
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
		void Execute(float dt);

		void SetConfig(RenderGraphConfig renderGraphConfig);

		void Finalize();

		Ref<RenderSource> FindSource(std::string sourceName);

		void AddRenderPassConfig(RenderPassConfig const& renderPassConfig);

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
