/******************************************************************************
/*!
\file       UniformBufferObjectOpenGLImpl.cpp
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

#include <Graphics/OpenGL/UniformBufferObjectOpenGLImpl.hpp>

#include <glad/glad.h>

namespace Borealis
{
	UniformBufferObjectOpenGLImpl::UniformBufferObjectOpenGLImpl(uint32_t size, uint32_t binding)
	{
		glGenBuffers(1, &mRendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBufferObjectOpenGLImpl::~UniformBufferObjectOpenGLImpl()
	{
		glDeleteBuffers(1, &mRendererID);
	}

	void UniformBufferObjectOpenGLImpl::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	int UniformBufferObjectOpenGLImpl::GetBlockSize(uint32_t shaderID, const char* blockName)
	{
		GLuint blockIndex = glGetUniformBlockIndex(shaderID, blockName);
		if (blockIndex == GL_INVALID_INDEX)
		{
			throw std::runtime_error(std::string("Uniform block '") + blockName + "' not found in the shader program!");
		}
		int blockSize = 0;
		glGetActiveUniformBlockiv(shaderID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

		return blockSize;
	}

	void UniformBufferObjectOpenGLImpl::BindToShader(uint32_t shaderID, const char* blockName, uint32_t binding)
	{
		glUseProgram(shaderID);
		GLuint blockIndex = glGetUniformBlockIndex(shaderID, blockName);
		if (blockIndex == GL_INVALID_INDEX)
		{
			throw std::runtime_error(std::string("Uniform block '") + blockName + "' not found in the shader program!");
		}
		glUniformBlockBinding(shaderID, blockIndex, binding);
		glUseProgram(0);
	}
}

