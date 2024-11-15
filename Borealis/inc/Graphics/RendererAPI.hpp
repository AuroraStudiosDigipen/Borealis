/******************************************************************************/
/*!
\file		RendererAPI.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the class for Graphics Renderer API

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RENDERER_API_HPP
#define RENDERER_API_HPP
#include <glm/glm.hpp>
#include <Graphics/VertexArray.hpp>
namespace Borealis
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1	
		};

		/*!***********************************************************************
			\brief
				Destructor of RendererAPI virtual class
		*************************************************************************/
		virtual ~RendererAPI() {};

		/*!***********************************************************************
			\brief
				Initializes the RendererAPI
		*************************************************************************/
		virtual void Init() = 0;

		/*!***********************************************************************
			\brief
				Clears the RendererAPI
		*************************************************************************/
		virtual void Clear() = 0;

		virtual void ClearStencil() = 0;

		/*!***********************************************************************
			\brief
				Sets the clear color of the RendererAPI
			\param[in] r
				The red component of the color
			\param[in] g
				The green component of the color
			\param[in] b
				The blue component of the color
			\param[in] a
				The alpha component of the color
		*************************************************************************/
		virtual void SetClearColor(const float& r, const float& g, const float& b, const float& a) = 0;

		/*!***********************************************************************
			\brief
				Sets the clear color of the RendererAPI
			\param[in] color
				The color to set the clear color to
		*************************************************************************/
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void SetStencilClear(int clear) = 0;

		/*!***********************************************************************
			\brief
				Draw Elements in the VAO
			\param[in] VAO
				The Vertex Array Object to draw
			\param[in] indexCount
				The number of indices to draw
		*************************************************************************/
		virtual void DrawElements(const Ref<VertexArray>& VAO, uint32_t indexCount = 0) = 0;

		/*!***********************************************************************
			\brief
				Draw Lines in the VAO
			\param[in] VAO
				The Vertex Array Object to draw
			\param[in] vertexCount
				The number of vertices to draw
		*************************************************************************/
		virtual void DrawLines(const Ref<VertexArray>& VAO, uint32_t vertexCount = 0) = 0;

		/*!***********************************************************************
			\brief
				Set the Viewport of the RendererAPI
			\param[in] x
				The x coordinate of the viewport
			\param[in] y
				The y coordinate of the viewport
			\param[in] width
				The width of the viewport
			\param[in] height
				The height of the viewport
		*************************************************************************/
		virtual void SetViewport(const uint32_t& x, const uint32_t& y, const uint32_t& width, const uint32_t& height) = 0;

		/*!***********************************************************************
			\brief
				Set the Line Thickness of the RendererAPI
			\param[in] thickness
				The thickness of the line
		*************************************************************************/
		virtual void SetLineThickness(const float& thickness) = 0;

		virtual void EnableBlend() = 0;
		virtual void DisableBlend() = 0;
		virtual void EnableDepthTest() = 0;
		virtual void DisableDepthTest() = 0;

		virtual void EnableBackFaceCull() = 0;
		virtual void EnableFrontFaceCull() = 0;

		virtual void EnableStencilTest() = 0;

		virtual void ConfigureStencilForHighlight() = 0;

		virtual void DisableStencilTest() = 0;

		virtual void EnableWireFrameMode() = 0;

		virtual void DisableWireFrameMode() = 0;

		virtual void IgnoreNextError() = 0;
		virtual unsigned GetError(std::string const& errorMsg) = 0;

		/*!***********************************************************************
			\brief
				Get the API being used
			\return
				The API being used
		*************************************************************************/
		inline static API GetAPI() { return sAPI; }
	private:
		static API sAPI; //!< The API being used
	}; // class RendererAPI

} // namespace Borealis
#endif