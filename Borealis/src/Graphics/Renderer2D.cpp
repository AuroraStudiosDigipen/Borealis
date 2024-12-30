/******************************************************************************/
/*!
\file		Renderer.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 10, 2024
\brief		Defines the class for Graphics Renderer2D

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Graphics/Renderer2D.hpp"
#include <Graphics/VertexArray.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/RenderCommand.hpp>

namespace Borealis
{
	struct QuadData
	{
		glm::vec3 Position;
		glm::vec4 Colour;
		glm::vec2 TexCoord;
		uint32_t TexIndex;
		float TilingFactor;

		//Editor only
		int EntityID;
	};

	struct SimpleQuadData
	{
		glm::vec3 Position;
		glm::vec3 NormalBuffer;
		glm::vec2 TexCoord;
	};

	struct CircleData
	{
		glm::vec3 WorldPosition;
		glm::vec2 LocalPosition;
		glm::vec4 Colour;
		float Thickness;
		float Fade;

		//Editor only
		int EntityID;
	};

	struct LineData
	{
		glm::vec3 Position;
		glm::vec4 Colour;
	};

	struct FontData
	{
		glm::vec3 Position;
		glm::vec4 Colour;
		glm::vec2 TexCoord;

		//Editor only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxCircles = 1000;
		static const uint32_t MaxLines = 1000000;
		static const uint32_t MaxFont = 10000;
		static const uint32_t MaxCircleVertices = MaxCircles * 4;
		static const uint32_t MaxLineVertices = MaxLines * 2;
		static const uint32_t MaxFontVertices = MaxFont * 4;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 16;


		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		QuadData* QuadBufferBase = nullptr;
		QuadData* QuadBufferPtr = nullptr;

		CircleData* CircleBufferBase = nullptr;
		CircleData* CircleBufferPtr = nullptr;

		LineData* LineBufferBase = nullptr;
		LineData* LineBufferPtr = nullptr;

		Ref<Texture2D> FontTexture;
		FontData* FontBufferBase = nullptr;
		FontData* FontBufferPtr = nullptr;
		
		glm::vec4 VertexPos[4];

		Ref<VertexArray> mQuadVAO;
		Ref<VertexBuffer> mQuadVBO;
		Ref<Shader> mQuadShader; 
		Ref<Texture2D> mWhiteTexture;

		Ref<VertexArray> mHighlightedQuadVAO;
		Ref<VertexBuffer> mHighlightedQuadVBO;

		Ref<VertexArray> mCircleVAO;
		Ref<VertexBuffer> mCircleVBO;
		Ref<Shader> mCircleShader;

		Ref<VertexArray> mLineVAO;
		Ref<VertexBuffer> mLineVBO;
		Ref<Shader> mLineShader;
		
		Ref<VertexArray> mFontVAO;
		Ref<VertexBuffer> mFontVBO;
		Ref<Shader> mFontShader;
		
		Renderer2D::Statistics mStats;
		uint32_t QuadIndexCount = 0;
		uint32_t CircleIndexCount = 0;
		uint32_t LineVertexCount = 0;
		uint32_t FontIndexCount = 0;
		uint32_t TextureSlotIndex = 1; // 0: White texture
	};
	
	
	static Renderer2DData* sData;

	void Renderer2D::Init()
	{
		PROFILE_FUNCTION();

		sData = new Renderer2DData();
		sData->mQuadVAO = VertexArray::Create();

		sData->mQuadVBO = VertexBuffer::Create(sData->MaxVertices * sizeof(QuadData));
		sData->mQuadVBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float2, "a_TexCoord"},
			{ ShaderDataType::Float, "a_TexIndex"},
			{ ShaderDataType::Float, "a_TilingFactor"},
			{ ShaderDataType::Int, "a_EntityID"}
			});
		sData->mQuadVAO->AddVertexBuffer(sData->mQuadVBO);

		sData->QuadBufferBase = new QuadData[sData->MaxVertices];
		sData->QuadBufferPtr = sData->QuadBufferBase;

		uint32_t* indices = new uint32_t[sData->MaxIndices];

		for (uint32_t i = 0, offset = 0; i < sData->MaxIndices; i += 6, offset += 4)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;
		}

		Ref<ElementBuffer> EBO = ElementBuffer::Create(indices, sData->MaxIndices);
		sData->mQuadVAO->SetElementBuffer(EBO);
		delete[] indices;

		sData->mHighlightedQuadVAO = VertexArray::Create();
		float quadVertices[] = {
			// Positions		  //normal  	   // TexCoords
			-0.5f,  0.5f, 0.0f,   0.f, 0.f, 0.f,   0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f,   0.f, 0.f, 0.f,   0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,   0.f, 0.f, 0.f,   1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,   0.f, 0.f, 0.f,   1.0f, 1.0f 
		};
		sData->mHighlightedQuadVBO = VertexBuffer::Create(quadVertices, 4 * sizeof(SimpleQuadData));
		sData->mHighlightedQuadVBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"},
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		sData->mHighlightedQuadVAO->AddVertexBuffer(sData->mHighlightedQuadVBO);

		uint32_t highlightIndices[6];

		highlightIndices[0] = 0;
		highlightIndices[1] = 1;
		highlightIndices[2] = 2;
		highlightIndices[3] = 0;
		highlightIndices[4] = 2;
		highlightIndices[5] = 3;

		Ref<ElementBuffer> HighlightedEBO = ElementBuffer::Create(highlightIndices, 6);
		sData->mHighlightedQuadVAO->SetElementBuffer(HighlightedEBO);

		//circle
		sData->mCircleVAO = VertexArray::Create();

		sData->mCircleVBO = VertexBuffer::Create(sData->MaxCircleVertices * sizeof(CircleData));
		sData->mCircleVBO->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float2, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float, "a_Thickness"},
			{ ShaderDataType::Float, "a_Float"},
			{ ShaderDataType::Int, "a_EntityID"}
			});
		sData->mCircleVAO->AddVertexBuffer(sData->mCircleVBO);
		sData->mCircleVAO->SetElementBuffer(EBO);
		sData->CircleBufferBase = new CircleData[sData->MaxCircleVertices];
		sData->CircleBufferPtr = sData->CircleBufferBase;

		//line
		sData->mLineVAO = VertexArray::Create();

		sData->mLineVBO = VertexBuffer::Create(sData->MaxLines * sizeof(LineData));
		sData->mLineVBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
			});
		sData->mLineVAO->AddVertexBuffer(sData->mLineVBO);
		sData->LineBufferBase = new LineData[sData->MaxLines];
		sData->LineBufferPtr = sData->LineBufferBase;

		//Font
		sData->mFontVAO = VertexArray::Create();

		sData->mFontVBO = VertexBuffer::Create(sData->MaxFont * sizeof(FontData));
		sData->mFontVBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float2, "a_TexCoord"},
			{ ShaderDataType::Int, "a_EntityID"}
			});
		sData->mFontVAO->AddVertexBuffer(sData->mFontVBO);

		sData->mFontVAO->SetElementBuffer(EBO);
		sData->FontBufferBase = new FontData[sData->MaxFont];
		sData->FontBufferPtr = sData->FontBufferBase;



		sData->mWhiteTexture = Texture2D::Create(TextureInfo{});
		uint32_t whiteTexData = 0xFFFFFFFF;
		sData->mWhiteTexture->SetData(&whiteTexData, sizeof(uint32_t));

		int samplers[Renderer2DData::MaxTextureSlots];
		for (int i = 0; i < Renderer2DData::MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		sData->mQuadShader = Shader::Create("engineResources/shaders/Renderer2D_Quad.glsl");
		sData->mQuadShader->Bind();
		sData->mQuadShader->Set("u_Texture", samplers, Renderer2DData::MaxTextureSlots);

		sData->mCircleShader = Shader::Create("engineResources/shaders/Renderer2D_Circle.glsl");
		sData->mLineShader = Shader::Create("engineResources/shaders/Renderer2D_Line.glsl");
		sData->mFontShader = Shader::Create("engineResources/shaders/Renderer2D_Font.glsl");

		sData->TextureSlots[0] = sData->mWhiteTexture;

		sData->VertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		sData->VertexPos[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		sData->VertexPos[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
		sData->VertexPos[3] = { -0.5f, 0.5f, 0.0f, 1.0f };
	}
	void Renderer2D::Free()
	{
		PROFILE_FUNCTION();
		delete[] sData->QuadBufferBase;	
		delete[] sData->CircleBufferBase;
		delete[] sData->LineBufferBase;
		delete[] sData->FontBufferBase;
		delete sData;	
	}
	void Renderer2D::Begin(const OrthographicCamera& camera)
	{
		PROFILE_FUNCTION();

		Renderer2D::Begin(camera.GetViewProjectionMatrix());
	}

	void Renderer2D::Begin(const EditorCamera& camera)
	{
		PROFILE_FUNCTION();
		//Consider merging into one shader file to avoid constant switching or some other method
		//profile first to see if its affecting performance
		Renderer2D::Begin(camera.GetViewProjectionMatrix());
	}


	void Renderer2D::Begin(const Camera& camera, const glm::mat4& transform)
	{
		PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjectionMatrix() *glm::inverse(transform);

		Renderer2D::Begin(viewProj);
	}

	void Renderer2D::Begin(glm::mat4 viewProj)
	{
		sData->mQuadShader->Bind();
		sData->mQuadShader->Set("u_ViewProjection", viewProj);

		sData->QuadIndexCount = 0;
		sData->QuadBufferPtr = sData->QuadBufferBase;

		sData->mCircleShader->Bind();
		sData->mCircleShader->Set("u_ViewProjection", viewProj);

		sData->CircleIndexCount = 0;
		sData->CircleBufferPtr = sData->CircleBufferBase;

		sData->mLineShader->Bind();
		sData->mLineShader->Set("u_ViewProjection", viewProj);

		sData->LineVertexCount = 0;
		sData->LineBufferPtr = sData->LineBufferBase;

		sData->mFontShader->Bind();
		sData->mFontShader->Set("u_ViewProjection", viewProj);

		sData->FontIndexCount = 0;
		sData->FontBufferPtr = sData->FontBufferBase;

		sData->TextureSlotIndex = 1;
	}

	void Renderer2D::End()
	{
		PROFILE_FUNCTION();
		Flush();

	}
	void Renderer2D::Flush()
	{
		PROFILE_FUNCTION();
		if (sData->QuadIndexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)sData->QuadBufferPtr - (uint8_t*)sData->QuadBufferBase);
			sData->mQuadVBO->SetData(sData->QuadBufferBase, size);
			for (uint32_t i = 0; i < sData->TextureSlotIndex; i++)
			{
				sData->TextureSlots[i]->Bind(i);
			}
			sData->mQuadShader->Bind();
			RenderCommand::DrawElements(sData->mQuadVAO, sData->QuadIndexCount);
			sData->mStats.DrawCalls++;
		}

		if (sData->CircleIndexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)sData->CircleBufferPtr - (uint8_t*)sData->CircleBufferBase);
			sData->mCircleVBO->SetData(sData->CircleBufferBase, size);
			sData->mCircleShader->Bind();
			RenderCommand::DrawElements(sData->mCircleVAO, sData->CircleIndexCount);
			sData->mStats.DrawCalls++;
		}

		if (sData->LineVertexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)sData->LineBufferPtr - (uint8_t*)sData->LineBufferBase);
			sData->mLineVBO->SetData(sData->LineBufferBase, size);
			sData->mLineShader->Bind();
			RenderCommand::DrawLines(sData->mLineVAO, sData->LineVertexCount);
			sData->mStats.DrawCalls++;
		}

		if (sData->FontIndexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)sData->FontBufferPtr - (uint8_t*)sData->FontBufferBase);
			sData->mFontVBO->SetData(sData->FontBufferBase, size);
			sData->FontTexture->Bind(0);
			sData->mFontShader->Bind();
			RenderCommand::DrawElements(sData->mFontVAO, sData->FontIndexCount);
			sData->mStats.DrawCalls++;
		}
	}

	static void FlushReset()
	{
		Renderer2D::End();
		sData->QuadBufferPtr = sData->QuadBufferBase;
		sData->QuadIndexCount = 0;
		sData->TextureSlotIndex = 1;
		sData->LineBufferPtr = sData->LineBufferBase;
		sData->LineVertexCount = 0;
	}
	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entityID)
	{
		PROFILE_FUNCTION();
		if (sprite.Texture)
		{
			DrawQuad(transform, sprite.Texture, sprite.TilingFactor, sprite.Colour, entityID);
		}
		else
			DrawQuad(transform, sprite.Colour, entityID);
	}

	void Renderer2D::DrawHighlightedSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, Ref<Shader> shader)
	{
		PROFILE_FUNCTION();
		DrawHighlightedQuad(transform, shader);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& colour, float thickness, float fade, int entityID)
	{
		PROFILE_FUNCTION();

		//if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
		//	FlushReset();

		for (int i = 0; i < 4; i++)
		{
			sData->CircleBufferPtr->WorldPosition = transform * sData->VertexPos[i];
			sData->CircleBufferPtr->LocalPosition = sData->VertexPos[i] * 2.0f;
			sData->CircleBufferPtr->Colour = colour;
			sData->CircleBufferPtr->Thickness = thickness;
			sData->CircleBufferPtr->Fade = fade;
			sData->CircleBufferPtr->EntityID = entityID;
			sData->CircleBufferPtr++;
		}

		sData->CircleIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
	{
		PROFILE_FUNCTION();

		glm::vec3 p0 = glm::vec3(position.x - size.x / 2, position.y - size.y / 2, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x / 2, position.y - size.y / 2, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x / 2, position.y + size.y / 2, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x / 2, position.y + size.y / 2, position.z);

		DrawLine(p0, p1, colour);
		DrawLine(p1, p2, colour);
		DrawLine(p2, p3, colour);
		DrawLine(p3, p0, colour);

	}
	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& colour)
	{
		PROFILE_FUNCTION();
		glm::vec3 lineVertices[4];

		for (int i = 0; i < 4; i++)
		{
			lineVertices[i] = transform * sData->VertexPos[i];
		}

		DrawLine(lineVertices[0], lineVertices[1], colour);
		DrawLine(lineVertices[1], lineVertices[2], colour);
		DrawLine(lineVertices[2], lineVertices[3], colour);
		DrawLine(lineVertices[3], lineVertices[0], colour);

	}

	struct LineInfo
	{
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec4 color;
	};

	static std::vector<LineInfo> lineQueue;
	static int lineCount = 0;

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour)
	{
		//PROFILE_FUNCTION();

		////if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
		////	FlushReset();

		//sData->LineBufferPtr->Position = p0;
		//sData->LineBufferPtr->Colour = colour;
		//sData->LineBufferPtr++;

		//sData->LineBufferPtr->Position = p1;
		//sData->LineBufferPtr->Colour = colour;
		//sData->LineBufferPtr++;

		//sData->LineVertexCount += 2;

		LineInfo info{ p0, p1, colour };
		lineQueue.push_back(info);
		lineCount++; 
		//BOREALIS_CORE_INFO("Drawline {}", lineCount);

	}

	void Renderer2D::DrawBox(const glm::vec3& pMin, const glm::vec3& pMax, const glm::vec4& colour)
	{
		glm::vec3 p1{ pMin.x, pMin.y, pMin.z };
		glm::vec3 p2{ pMax.x, pMin.y, pMin.z };
		glm::vec3 p3{ pMax.x, pMax.y, pMin.z };
		glm::vec3 p4{ pMin.x, pMax.y, pMin.z };
		glm::vec3 p5{ pMin.x, pMin.y, pMax.z };
		glm::vec3 p6{ pMax.x, pMin.y, pMax.z };
		glm::vec3 p7{ pMax.x, pMax.y, pMax.z };
		glm::vec3 p8{ pMin.x, pMax.y, pMax.z };

		DrawLine(p1, p2, colour);
		DrawLine(p2, p3, colour);
		DrawLine(p3, p4, colour);
		DrawLine(p4, p1, colour);
		DrawLine(p5, p6, colour);
		DrawLine(p6, p7, colour);
		DrawLine(p7, p8, colour);
		DrawLine(p8, p5, colour);
		DrawLine(p1, p5, colour);
		DrawLine(p2, p6, colour);
		DrawLine(p3, p7, colour);
		DrawLine(p4, p8, colour);
	}

	void Renderer2D::DrawLineFromQueue()
	{
		PROFILE_FUNCTION();



		for (LineInfo const& info : lineQueue)
		{
			if (sData->LineVertexCount + 2 >= Renderer2DData::MaxLineVertices)
			{
				FlushReset();
			}
			sData->LineBufferPtr->Position = info.p0;
			sData->LineBufferPtr->Colour = info.color;
			sData->LineBufferPtr++;

			sData->LineBufferPtr->Position = info.p1;
			sData->LineBufferPtr->Colour = info.color;
			sData->LineBufferPtr++;

			sData->LineVertexCount += 2;
		}
	}

	void Renderer2D::ClearDrawQueue()
	{
		lineQueue.clear();
		//BOREALIS_CORE_INFO("Drawline {}", lineCount);
		lineCount = 0;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawRotatedQuad({ position.x,position.y,0 }, rotation, size, colour);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size,  const glm::vec4& colour)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount+6 >= Renderer2DData::MaxIndices)
			FlushReset();

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		constexpr uint32_t texIndex = 0;
		constexpr glm::vec2 texCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = colour;
			sData->QuadBufferPtr->TexCoord = texCoords[i];
			sData->QuadBufferPtr->TexIndex = texIndex;
			sData->QuadBufferPtr->TilingFactor = 1.0f;
			sData->QuadBufferPtr++;
		}
		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		DrawRotatedQuad({ position.x,position.y,0 }, rotation, size, texture, tilingFactor, tintColour);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		uint32_t textureUnit = 0;

		for (uint32_t i = 1; i < sData->TextureSlotIndex; i++)
		{
			if (*sData->TextureSlots[i].get() == *texture.get())
			{
				textureUnit = i;
				break;
			}
		}

		if (textureUnit == 0)
		{
			if (sData->TextureSlotIndex + 1 == sData->MaxTextureSlots)
			{
				FlushReset();
			}
			textureUnit = sData->TextureSlotIndex;
			sData->TextureSlots[textureUnit] = texture;
			sData->TextureSlotIndex++;
		}

		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), position) * 
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) * 
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		constexpr glm::vec4 colour = { 1.0f,1.0f,1.0f,1.0f };
		constexpr glm::vec2 texCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = colour;
			sData->QuadBufferPtr->TexCoord = texCoords[i];
			sData->QuadBufferPtr->TexIndex = textureUnit;
			sData->QuadBufferPtr->TilingFactor = tilingFactor;
			sData->QuadBufferPtr++;
		}

		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		DrawRotatedQuad({ position.x,position.y,0 }, rotation, size, subtexture, tilingFactor, tintColour);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		uint32_t textureUnit = 0;

		for (uint32_t i = 1; i < sData->TextureSlotIndex; i++)
		{
			if (*sData->TextureSlots[i].get() == *subtexture->GetTexture().get())
			{
				textureUnit = i;
				break;
			}
		}

		if (textureUnit == 0)
		{
			if (sData->TextureSlotIndex + 1 == sData->MaxTextureSlots)
			{
				FlushReset();
			}
			textureUnit = sData->TextureSlotIndex;
			sData->TextureSlots[textureUnit] = subtexture->GetTexture();
			sData->TextureSlotIndex++;
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		constexpr glm::vec4 colour = { 1.0f,1.0f,1.0f,1.0f };

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = colour;
			sData->QuadBufferPtr->TexCoord = subtexture->GetTexCoords()[i];
			sData->QuadBufferPtr->TexIndex = textureUnit;
			sData->QuadBufferPtr->TilingFactor = tilingFactor;
			sData->QuadBufferPtr++;
		}

		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, int entityID, float size, glm::vec4 colour)
	{
		if (!font) font = Font::GetDefaultFont();

		Ref<Texture2D> fontAtlas = font->GetAtlasTexture();

		if (!fontAtlas) return;

		sData->FontTexture = fontAtlas;

		FontInfo const& fontInfo = font->GetFontInfo();

		double x = 0.0;
		double fsScale = size / (fontInfo.ascenderY - fontInfo.descenderY);

		double y = 0.0;

		for (int i{}; i < string.size(); i++)
		{
			char character = string[i];

			if (character == '\r')
				continue;

			if (character == '\n' || string[i + 1] == '\n')
			{
				x = 0;
				//y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				continue;
			}

			FontGlyph glyph = fontInfo.glyphs.at(character);

			glm::vec2 texCoordMin((float)glyph.altasBound.left, (float)glyph.altasBound.bottom);
			glm::vec2 texCoordMax((float)glyph.altasBound.right, (float)glyph.altasBound.top);

			glm::vec2 quadMin((float)glyph.planeBound.left, (float)glyph.planeBound.bottom);
			glm::vec2 quadMax((float)glyph.planeBound.right, (float)glyph.planeBound.top);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			sData->FontBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			sData->FontBufferPtr->Colour = colour;
			sData->FontBufferPtr->TexCoord = texCoordMin;
			sData->FontBufferPtr->EntityID = entityID;
			sData->FontBufferPtr++;

			sData->FontBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			sData->FontBufferPtr->Colour = colour;
			sData->FontBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			sData->FontBufferPtr->EntityID = entityID;
			sData->FontBufferPtr++;

			sData->FontBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			sData->FontBufferPtr->Colour = colour;
			sData->FontBufferPtr->TexCoord = texCoordMax;
			sData->FontBufferPtr->EntityID = entityID;
			sData->FontBufferPtr++;

			sData->FontBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			sData->FontBufferPtr->Colour = colour;
			sData->FontBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			sData->FontBufferPtr->EntityID = entityID;
			sData->FontBufferPtr++;

			sData->FontIndexCount += 6;
			sData->mStats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph.advance;
				char nextCharacter = string[i + 1];
				//fontGeometry.getAdvance(advance, character, nextCharacter);
				advance = fontInfo.kernings.at({character, nextCharacter});

				x += fsScale * advance;// +textParams.Kerning;
			}
		}
	}

	void Renderer2D::DrawString(TextComponent& text, TransformComponent& trans, int entityID)
	{
		DrawString(text.text, text.font, trans.GetTransform(), entityID, (float)text.fontSize, text.colour);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawQuad({ position.x,position.y,0 }, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, int entityID)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		constexpr glm::vec2 texCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };
		constexpr uint32_t texIndex = 0;

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = colour;
			sData->QuadBufferPtr->TexCoord = texCoords[i];
			sData->QuadBufferPtr->TexIndex = texIndex;
			sData->QuadBufferPtr->TilingFactor = 1.0f;
			sData->QuadBufferPtr->EntityID = entityID;
			sData->QuadBufferPtr++;
		}

		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
	
		DrawQuad(transform, colour);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		DrawQuad({ position.x,position.y,0 }, size, texture, tilingFactor, tintColour);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		PROFILE_FUNCTION();
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const float& tilingFactor, const glm::vec4& tint, int entityID)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		uint32_t textureUnit = 0;

		for (uint32_t i = 1; i < sData->TextureSlotIndex; i++)
		{
			if (*sData->TextureSlots[i].get() == *texture.get())
			{
				textureUnit = i;
				break;
			}
		}

		if (textureUnit == 0)
		{
			if (sData->TextureSlotIndex + 1 == sData->MaxTextureSlots)
			{
				FlushReset();
			}
			textureUnit = sData->TextureSlotIndex;
			sData->TextureSlots[textureUnit] = texture;
			sData->TextureSlotIndex++;
		}

		
		constexpr glm::vec2 texCoords[] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = tint;
			sData->QuadBufferPtr->TexCoord = texCoords[i];
			sData->QuadBufferPtr->TexIndex = textureUnit;
			sData->QuadBufferPtr->TilingFactor = tilingFactor;
			sData->QuadBufferPtr->EntityID = entityID;
			sData->QuadBufferPtr++;
		}

		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::DrawHighlightedQuad(const glm::mat4& transform, Ref<Shader> shader)
	{
		shader->Bind();

		shader->Set("u_ModelTransform", transform);
		RenderCommand::DrawElements(sData->mHighlightedQuadVAO, 6);

		shader->Unbind();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		DrawQuad({ position.x,position.y,0 }, size, subtexture, tilingFactor, tintColour);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float& tilingFactor, const glm::vec4& tintColour)
	{
		PROFILE_FUNCTION();

		if (sData->QuadIndexCount + 6 >= Renderer2DData::MaxIndices)
			FlushReset();

		uint32_t textureUnit = 0;

		for (uint32_t i = 1; i < sData->TextureSlotIndex; i++)
		{
			if (*sData->TextureSlots[i].get() == *subtexture->GetTexture().get())
			{
				textureUnit = i;
				break;
			}
		}

		if (textureUnit == 0)
		{
			if (sData->TextureSlotIndex + 1 == sData->MaxTextureSlots)
			{
				FlushReset();
			}
			textureUnit = sData->TextureSlotIndex;
			sData->TextureSlots[textureUnit] = subtexture->GetTexture();
			sData->TextureSlotIndex++;
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		constexpr glm::vec4 colour = { 1.0f,1.0f,1.0f,1.0f };

		for (int i = 0; i < 4; i++)
		{
			sData->QuadBufferPtr->Position = transform * sData->VertexPos[i];
			sData->QuadBufferPtr->Colour = colour;
			sData->QuadBufferPtr->TexCoord = subtexture->GetTexCoords()[i];
			sData->QuadBufferPtr->TexIndex = textureUnit;
			sData->QuadBufferPtr->TilingFactor = tilingFactor;
			sData->QuadBufferPtr++;
		}

		sData->QuadIndexCount += 6;

		sData->mStats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		sData->mStats.QuadCount = 0;
		sData->mStats.DrawCalls = 0;
	}

	Renderer2D::Statistics& Renderer2D::GetStats()
	{
		return sData->mStats;
	}

}