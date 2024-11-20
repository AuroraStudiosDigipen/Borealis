/******************************************************************************
/*!
\file       PixelBuffer.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Graphics/PixelBuffer.hpp>

#include <Core/Core.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/OpenGL/PixelBufferOpenGLImpl.hpp>

namespace Borealis
{
	Ref<PixelBuffer> PixelBuffer::Create(const PixelBufferProperties& properties)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BOREALIS_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case RendererAPI::API::OpenGL:  return MakeRef<OpenGLPixelBuffer>(properties);
		}

		BOREALIS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

