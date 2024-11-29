/******************************************************************************/
/*!
\file		Renderer2D.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 10, 2024
\brief		Declares the class for Graphics Renderer2D

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef RENDERER2D_HPP
#define RENDERER2D_HPP
#include <Graphics/OrthographicCamera.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/SubTexture2D.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/EditorCamera.hpp>
#include <Scene/Components.hpp>
namespace Borealis
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Free();

		static void Begin(const OrthographicCamera& camera);
		static void Begin(const EditorCamera& camera);
		static void Begin(const Camera& camera, const glm::mat4& transform);
		static void Begin(glm::mat4 viewProj);
		static void End();
		static void Flush();

		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entityID = -1);

		static void DrawHighlightedSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, Ref<Shader> shader);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& colour, float thickness = 1.f, float fade = 0.005f, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour); // Position and Size
		static void DrawRect(const glm::mat4& transform, const glm::vec4& colour); // Transform matrix

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,const float& tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,const float& tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4 & = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4 & = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4& tint = {1,1,1,1}, int entityID = -1);

		static void DrawHighlightedQuad(const glm::mat4& transform, Ref<Shader> shader);

		static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4& = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4& = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<SubTexture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4 & = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<SubTexture2D>& texture, const float& tilingFactor = 1.f, const glm::vec4 & = glm::vec4(1.0f));

		static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, int entityID, float size = 1.f, glm::vec4 colour = {1.f,1.f,1.f,1.f});
		static void DrawString(TextComponent& text, TransformComponent& trans, int entityID);
		static void DrawBox(const glm::vec3& pMin, const glm::vec3& pMax, const glm::vec4& colour);

		static void DrawLineFromQueue();
		static void ClearDrawQueue();

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics& GetStats();
	};

}
#endif