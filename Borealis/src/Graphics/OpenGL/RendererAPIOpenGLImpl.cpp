/******************************************************************************/
/*!
\file		RendererAPIOpenGLImpl.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Defines the class for Graphics Renderer API

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <glad/glad.h>
#include "Graphics/OpenGL/RendererAPIOpenGLImpl.hpp"

#include "Core/ApplicationManager.hpp"
namespace Borealis
{
	void OpenGLRendererAPI::Init()
	{
		PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error");
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const float& r, const float& g, const float& b, const float& a)
	{
		glClearColor(r, g, b, a);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::SetStencilClear(int clear)
	{
		glClearStencil(clear);
	}
	void OpenGLRendererAPI::DrawElements(const Ref<VertexArray>& VAO, uint32_t count)
	{
		VAO->Bind();
		uint32_t indexCount = count ? count : VAO->GetElementBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& VAO, uint32_t vertexCount)
	{
		VAO->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}
	void OpenGLRendererAPI::SetViewport (const uint32_t& x, const uint32_t& y, const uint32_t& width, const uint32_t& height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::SetLineThickness(const float& thickness)
	{
		glLineWidth(thickness);
	}

	void OpenGLRendererAPI::BindBackBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, ApplicationManager::Get().GetWindow()->GetWidth(), ApplicationManager::Get().GetWindow()->GetHeight());
	}

	void OpenGLRendererAPI::EnableBlend()
	{
		glEnable(GL_BLEND);
	}

	void OpenGLRendererAPI::DisableBlend()
	{
		glDisable(GL_BLEND);
	}

	void OpenGLRendererAPI::ConfigureBlendForTransparency(TransparencyStage stage)
	{
		if (stage == TransparencyStage::ACCUMULATION)
		{
			glBlendFunci(0, GL_ONE, GL_ONE);
			glBlendFunci(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			glBlendEquation(GL_FUNC_ADD);
		}
		else if(stage == TransparencyStage::REVEALAGE)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::ConfigureDepthFunc(DepthFunc func)
	{
		switch (func)
		{
		case Borealis::DepthFunc::DepthLess:
			glDepthFunc(GL_LESS);
			break;
		case Borealis::DepthFunc::DepthLEqual:
			glDepthFunc(GL_LEQUAL);
			break;
		default:
			break;
		}
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetDepthMask(bool depthMask)
	{
		if (depthMask)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::EnableBackFaceCull()
	{
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::EnableFrontFaceCull()
	{
		glCullFace(GL_FRONT);
	}

	void OpenGLRendererAPI::EnableStencilTest()
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
	}

	void OpenGLRendererAPI::ConfigureStencilForHighlight()
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	void OpenGLRendererAPI::DisableStencilTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::EnableWireFrameMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::DisableWireFrameMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void OpenGLRendererAPI::EnablePolygonOffset()
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	void OpenGLRendererAPI::SetPolygonOffset(float factor, float units)
	{
		glPolygonOffset(factor, units);
	}

	void OpenGLRendererAPI::DisablePolygonOffset()
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	void OpenGLRendererAPI::IgnoreNextError()
	{
		ignoreNextError = true;
	}

	unsigned OpenGLRendererAPI::GetError(std::string const& errorMsg)
	{
		unsigned err{};
		err = glGetError();
		if (ignoreNextError)
			ignoreNextError = false;
		else
		{
			if (err != GL_NO_ERROR)
				BOREALIS_CORE_WARN("Error with opengl no: {}", err);
			//BOREALIS_CORE_ASSERT(err == GL_NO_ERROR, errorMsg);
		}

		return err;
	}
}