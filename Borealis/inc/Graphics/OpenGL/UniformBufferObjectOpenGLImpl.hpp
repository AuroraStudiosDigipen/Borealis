/******************************************************************************
/*!
\file       UniformBufferObjectOpenGLImpl.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       January 14, 2025
\brief      Declares

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef UniformBufferObjectOpenGLImpl_HPP
#define UniformBufferObjectOpenGLImpl_HPP

#include <Graphics/UniformBufferObject.hpp>

namespace Borealis
{
	class UniformBufferObjectOpenGLImpl : public UniformBufferObject
	{
	public:
		UniformBufferObjectOpenGLImpl(uint32_t size, uint32_t binding);
		virtual ~UniformBufferObjectOpenGLImpl();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t mRendererID = 0;
	};
}

#endif
