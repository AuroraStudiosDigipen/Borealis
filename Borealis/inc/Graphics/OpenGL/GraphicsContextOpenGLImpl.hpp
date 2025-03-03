/******************************************************************************/
/*!
\file		GraphicsContextOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the OpenGl class for Graphics Context

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef GRAPHICS_CONTEXT_OPENGL_IMPL_HPP
#define GRAPHICS_CONTEXT_OPENGL_IMPL_HPP
#include "Graphics/GraphicsContext.hpp" 

struct GLFWwindow;

namespace Borealis
{
	class OpenGLContext : public GraphicsContext
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor for OpenGLContext
			\param windowHandle
				The window handle for the context
		*************************************************************************/
		OpenGLContext(GLFWwindow* windowHandle);

		/*!***********************************************************************
			\brief
				Initialise the context
		*************************************************************************/
		virtual void Init() override;

		/*!***********************************************************************
			\brief
				Swap the buffers
		*************************************************************************/
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* mWindowHandle; //!< The window handle for the context
	}; // class OpenGLContext
} // namespace Borealis

#endif