/******************************************************************************
/*!
\file       PixelBufferOpenGLImpl.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 14, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef PixelBufferOpenGLImpl_HPP
#define PixelBufferOpenGLImpl_HPP

#include <Core/Core.hpp>
#include <Graphics/PixelBuffer.hpp>

namespace Borealis
{
	class OpenGLPixelBuffer : public PixelBuffer
	{
	public:
		OpenGLPixelBuffer(PixelBufferProperties const& props);
		~OpenGLPixelBuffer();

		void ReadTexture(uint32_t index) override;

		int ReadPixel(int x, int y) override;

		void Bind() override;

		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		const PixelBufferProperties& GetProperties() const override;

		void Recreate();
	private:
		PixelBufferProperties mProps;
		uint32_t mRendererID;
	};
}

#endif
