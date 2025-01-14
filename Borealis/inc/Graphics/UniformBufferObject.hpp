/******************************************************************************
/*!
\file       UniformBufferObject.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       January 14, 2025
\brief      Declares

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef UniformBufferObject_HPP
#define UniformBufferObject_HPP

#include <Core/Core.hpp>

namespace Borealis
{
	class UniformBufferObject
	{
	public:
		virtual ~UniformBufferObject() {}
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<UniformBufferObject> Create(uint32_t size, uint32_t binding);
	};
}

#endif
