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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::SetClearColor(const float& r, const float& g, const float& b, const float& a)
	{
		glClearColor(r, g, b, a);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
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

	void OpenGLRendererAPI::EnableBlend()
	{
		glEnable(GL_BLEND);
	}

	void OpenGLRendererAPI::DisableBlend()
	{
		glDisable(GL_BLEND);
	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
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
			BOREALIS_CORE_ASSERT(err == GL_NO_ERROR, errorMsg);
		}

		return err;
	}
}