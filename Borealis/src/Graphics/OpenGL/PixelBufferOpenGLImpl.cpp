/******************************************************************************
/*!
\file       PixelBufferOpenGLImpl.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 14, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Graphics/OpenGL/PixelBufferOpenGLImpl.hpp>

#include <glad/glad.h>

#include <Core/LoggerSystem.hpp>

namespace Borealis
{
	static const uint32_t s_MaxPixelbufferSize = 16384;

	OpenGLPixelBuffer::OpenGLPixelBuffer(PixelBufferProperties const& props) : mProps(props), mRendererID(0)
	{
		Recreate();
	}

	OpenGLPixelBuffer::~OpenGLPixelBuffer()
	{
		//TODO fillin
	}

	void OpenGLPixelBuffer::ReadTexture(uint32_t index)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
		glReadPixels(0, 0, mProps.Width, mProps.Height, GL_RED_INTEGER, GL_INT, 0);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Pixel Buffer Error");
	}

	int OpenGLPixelBuffer::ReadPixel(int x, int y)
	{
		if (x < 0 || x >= (int)mProps.Width || y < 0 || y >= (int)mProps.Height) 
		{
			return -1; // Return an invalid ID if out of bounds
		}

		glBindBuffer(GL_PIXEL_PACK_BUFFER, mRendererID);

		GLint* ptr = (GLint*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		GLint entityID = -1;
		if (ptr) {
			entityID = entityID = ptr[y * mProps.Width + x];
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}

		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Pixel Buffer Error");

		return entityID;
	}

	void OpenGLPixelBuffer::Bind()
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, mRendererID);
	}

	void OpenGLPixelBuffer::Unbind()
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}

	void OpenGLPixelBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxPixelbufferSize || height > s_MaxPixelbufferSize)
		{
			BOREALIS_CORE_WARN("Framebuffer size is invalid, Width: {}, Height {}", width, height);
			return;
		}
		mProps.Width = width;
		mProps.Height = height;
		Recreate();
	}

	const PixelBufferProperties& OpenGLPixelBuffer::GetProperties() const
	{
		return mProps;
	}

	void OpenGLPixelBuffer::Recreate()
	{
		glGenBuffers(1, &mRendererID);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, mRendererID);
		glBufferData(GL_PIXEL_PACK_BUFFER, mProps.Width * mProps.Height * sizeof(GLint), nullptr, GL_DYNAMIC_READ); //see if there's difference between GL_STREAM_READ and GL_DYNAMIC_READ
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Pixel Buffer Creation Failed");
	}
}

