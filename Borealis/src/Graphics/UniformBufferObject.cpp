/******************************************************************************
/*!
\file       UniformBufferObject.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       January 14, 2025
\brief      Defines

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/UniformBufferObject.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/OpenGL/UniformBufferObjectOpenGLImpl.hpp>

namespace Borealis
{
	Ref<UniformBufferObject> UniformBufferObject::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "No render api");
		case RendererAPI::API::OpenGL: return MakeRef<UniformBufferObjectOpenGLImpl>(size, binding);
		}

		BOREALIS_CORE_ASSERT(false, "Unknown API");
		return nullptr;
	}

	void UniformBufferObject::BindToShader(uint32_t shaderID, const char* blockName, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "No render api");
		case RendererAPI::API::OpenGL:UniformBufferObjectOpenGLImpl::BindToShader(shaderID, blockName, binding); return;
		}
	}
}

