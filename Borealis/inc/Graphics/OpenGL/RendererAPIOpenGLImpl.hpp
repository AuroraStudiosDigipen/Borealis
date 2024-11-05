/******************************************************************************/
/*!
\file		RendererAPIOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the class for Graphics Renderer API

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RENDERER_API_OPENGL_IMPL_HPP
#define RENDERER_API_OPENGL_IMPL_HPP
#include "Graphics/RendererAPI.hpp"

namespace Borealis
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:

		/*!***********************************************************************
			\brief
				Init the RendererAPI
		*************************************************************************/
		void Init() override;

		/*!***********************************************************************
			\brief
				Clear the RendererAPI
		*************************************************************************/
		void Clear() override;

		/*!***********************************************************************
			\brief
				Set the clear color of the RendererAPI
			\param r
				The red component of the color
			\param g
				The green component of the color
			\param b
				The blue component of the color
			\param a
				The alpha component of the color
		*************************************************************************/
		void SetClearColor(const float& r, const float& g, const float& b, const float& a) override;

		/*!***********************************************************************
			\brief
				Set the clear color of the RendererAPI
			\param color
				The color to set the clear color to
		*************************************************************************/
		void SetClearColor(const glm::vec4& color) override;

		/*!***********************************************************************
			\brief
				Draw the elements of the VertexArray
			\param VAO
				The VertexArray to draw
			\param indexCount
				The number of indices to draw
		*************************************************************************/
		void DrawElements(const Ref<VertexArray>& VAO, uint32_t indexCount = 0) override;

		/*!***********************************************************************
			\brief
				Draw the lines of the vertex array
			\param VAO
				The VertexArray to draw
			\param vertexCount
				The number of vertices to draw
		*************************************************************************/
		void DrawLines(const Ref<VertexArray>& VAO, uint32_t vertexCount = 0) override;

		/*!***********************************************************************
			\brief
				Set the Viewport of the RendererAPI
			\param[in] x
				The x position of the viewport
			\param[in] y
				The y position of the viewport
			\param[in] width
				The width of the viewport
			\param[in] height
				The height of the viewport
		*************************************************************************/
		void SetViewport(const uint32_t& x, const uint32_t& y, const uint32_t& width, const uint32_t& height) override;

		/*!***********************************************************************
			\brief
				Set the line thickness of the RendererAPI
			\param thickness
				The thickness of the line
		*************************************************************************/
		void SetLineThickness(const float& thickness) override;

		void IgnoreNextError() override;

		unsigned GetError(std::string const& errorMsg) override;

	private:
		bool ignoreNextError = false;
	}; // class OpenGLRendererAPI
} // namespace Borealis

#endif