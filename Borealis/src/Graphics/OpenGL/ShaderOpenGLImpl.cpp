/******************************************************************************/
/*!
\file		ShaderOpenGLImpl.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Implements the functions for OpenGL Shader Class of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <Graphics/OpenGL/ShaderOpenGLImpl.hpp>
#include <Core/LoggerSystem.hpp>

namespace Borealis
{
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		PROFILE_FUNCTION();
		std::string shaderSrc = ReadFile(filepath);
		auto shaderSources = PreProcess(shaderSrc);
		Compile(shaderSources);

		size_t lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		size_t lastDot = filepath.rfind('.');
		lastDot = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		mName = filepath.substr(lastSlash, lastDot);
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) : mName(name)
	{
		PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		Compile(sources);
	}
	OpenGLShader::~OpenGLShader()
	{
		PROFILE_FUNCTION();
		glDeleteProgram(mRendererID);
	}
	void OpenGLShader::Bind() const
	{
		PROFILE_FUNCTION();
		glUseProgram(mRendererID);
	}
	void OpenGLShader::Unbind() const
	{
		PROFILE_FUNCTION();
		glUseProgram(0);
	}
	void OpenGLShader::Set(const char* name, const int& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const int* values, const uint32_t& count)
	{
		PROFILE_FUNCTION();

		PushUniform(name, values, count);
	}
	void OpenGLShader::Set(const char* name, const float& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const glm::vec2& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const glm::vec3& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const glm::vec4& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const glm::mat3& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::Set(const char* name, const glm::mat4& value)
	{
		PROFILE_FUNCTION();

		PushUniform(name, value);
	}
	void OpenGLShader::PushUniform(const char* name, const int& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform1i(location, value);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const int* values, const uint32_t count)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform1iv(location, count, values);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const float& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform1f(location, value);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const glm::vec2& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform2f(location, value.x, value.y);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const glm::vec3& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform3f(location, value.x, value.y, value.z);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const glm::vec4& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniform4f(location, value.x, value.y, value.z, value.w);
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const glm::mat3& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	void OpenGLShader::PushUniform(const char* name, const glm::mat4& value)
	{
		auto location = glGetUniformLocation(mRendererID, name);
		if (location == -1)
		{
			BOREALIS_CORE_ERROR("Invalid uniform name: {}", name);
		}
		glUniformMatrix4fv(glGetUniformLocation(mRendererID, name), 1, GL_FALSE, glm::value_ptr(value));
		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error pushing uniform into shader");
	}
	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;

		BOREALIS_CORE_ERROR("Invalid Shader Type: {}", type);
		return 0;
	}
	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		PROFILE_FUNCTION();

		std::ifstream file(filepath, std::ios::in | std::ios::binary);
		std::string result;
		if (file)
		{
			file.seekg(0, std::ios::end);
			result.resize(file.tellg());
			file.seekg(0, std::ios::beg);
			file.read(&result[0], result.size());
			file.close();
		}
		else
		{
			BOREALIS_CORE_ERROR("Shader file {} not found", filepath);
		}
		return result;
	}
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> result;

		const char* typeStr = "#type";
		size_t typeStrLen = strlen(typeStr);
		size_t pos = source.find(typeStr, 0);
		while (pos != std::string::npos)
		{
			size_t endOfLine = source.find("\r\n", pos);
			if (endOfLine == std::string::npos)
				endOfLine = source.find("\n", pos);
			BOREALIS_CORE_ASSERT(endOfLine != std::string::npos, "Syntax Error when Preprocessing shader");
			
			size_t begin = pos + typeStrLen + 1;
			std::string type = source.substr(begin, endOfLine - begin);
			BOREALIS_CORE_ASSERT(ShaderTypeFromString(type) != 0, "Invalid Shader type specified");
			
			size_t nextLinePos = source.find_first_not_of("\r\n", endOfLine);
			if (nextLinePos == std::string::npos)
				nextLinePos = source.find_first_not_of("\n", nextLinePos);
			pos = source.find(typeStr, nextLinePos);
			result[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return result;
	}
	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		BOREALIS_CORE_ASSERT(shaderSources.size() <= 3, "More than 3 shaders detected in 1 glsl");
		std::array<GLenum, 3> shaderIDs{};
		int index = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const char* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.
				BOREALIS_CORE_ERROR("Shader Compilation Error: {}", infoLog.data());

				// In this simple program, we'll just leave
				break;
			}
			glAttachShader(program, shader);
			shaderIDs[index++] = shader;
		}
	
		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			for (auto id : shaderIDs)
			{
				if (id == 0) continue;
				glDeleteShader(id);
			}
			// Use the infoLog as you see fit.
			BOREALIS_CORE_ERROR("Shader Program Linking Error: {}", infoLog.data());
			// In this simple program, we'll just leave
			return;
		}

		for (auto id : shaderIDs)
		{
			if (id == 0) continue;
			glDetachShader(program,id);
		}
		mRendererID = program;
	}
}