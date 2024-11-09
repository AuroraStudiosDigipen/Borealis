/******************************************************************************/
/*!
\file		RenderCommand.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the wrapper class for Graphics Render Commmand

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RENDER_COMMAND_HPP
#define RENDER_COMMAND_HPP
#include <memory>
#include <Graphics/RendererAPI.hpp>

namespace Borealis
{
	class RenderCommand
	{
	public:

		/*!***********************************************************************
			\brief
				Initialises the Renderer
		*************************************************************************/
		inline static void Init() {
			PROFILE_FUNCTION();
			sRendererAPI->Init(); }
		/*!***********************************************************************
			\brief
				Draw Elements in the VAO
			\param[in] VAO
				The Vertex Array Object to draw
			\param[in] count
				The number of elements to draw
		*************************************************************************/
		inline static void DrawElements(const Ref<VertexArray>& VAO, uint32_t count = 0) { sRendererAPI->DrawElements(VAO, count); }

		/*!***********************************************************************
			\brief
				Draw Lines in the VAO
			\param[in] VAO
				The Vertex Array Object to draw
			\param[in] count
				The number of elements to draw
		*************************************************************************/
		inline static void DrawLines(const Ref<VertexArray>& VAO, uint32_t count = 0) { sRendererAPI->DrawLines(VAO, count); }

		/*!***********************************************************************
			\brief
				Clears the Renderer
		*************************************************************************/
		inline static void Clear() { sRendererAPI->Clear(); }

		/*!***********************************************************************
			\brief
				Clears the Renderer with a color
			\param[in] r
				The red component of the color
			\param[in] g
				The green component of the color
			\param[in] b
				The blue component of the color
			\param[in] a
				The alpha component of the color
		*************************************************************************/
		inline static void SetClearColor(const float& r, const float& g, const float& b, const float& a) { sRendererAPI->SetClearColor(r, g, b, a); }

		/*!***********************************************************************
			\brief
				Clears the Renderer with a color
			\param[in] color
				The color to clear the renderer with
		*************************************************************************/
		inline static void SetClearColor(const glm::vec4& color) { sRendererAPI->SetClearColor(color); }

		/*!***********************************************************************
			\brief
				Sets the Viewport
			\param[in] x
				The x position of the viewport
			\param[in] y
				The y position of the viewport
			\param[in] width
				The width of the viewport
			\param[in] height
				The height of the viewport
		*************************************************************************/
		inline static void SetViewport(const uint32_t& x, const uint32_t& y, const uint32_t& width, const uint32_t& height) { sRendererAPI->SetViewport(x, y, width, height); }

		/*!***********************************************************************
			\brief
				Sets the line thickness
			\param[in] thickness
				The thickness of the line
		*************************************************************************/
		inline static void SetLineThickness(const float& thickness) { sRendererAPI->SetLineThickness(thickness); }

		inline static void EnableBlend() { sRendererAPI->EnableBlend(); }

		inline static void DisableBlend() { sRendererAPI->DisableBlend(); }

		inline static void EnableDepthTest() { sRendererAPI->EnableDepthTest(); }

		inline static void DisableDepthTest() { sRendererAPI->DisableDepthTest(); }

		inline static void EnableBackFaceCull() { sRendererAPI->EnableBackFaceCull(); }

		inline static void EnableFrontFaceCull() { sRendererAPI->EnableFrontFaceCull(); }

		inline static void IgnoreNextError() { sRendererAPI->IgnoreNextError(); }
		inline static unsigned GetError(std::string const& errorMsg) { return sRendererAPI->GetError(errorMsg); }
	private:
		static std::unique_ptr<RendererAPI> sRendererAPI; /*!< The Renderer API */
	}; // class RenderCommand
} // namespace Borealis
#endif