/******************************************************************************/
/*!
\file		ShaderOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Declares the functions for OpenGL Shader Class of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SHADER_OPENGL_IMPLHPP
#define SHADER_OPENGL_IMPLHPP
#include <Graphics/Shader.hpp>
namespace Borealis
{
	class OpenGLShader : public Shader
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor for the OpenGL Shader class
			\param filepath
				The file path to the shader file
		*************************************************************************/
		OpenGLShader(const std::string& filepath);

		/*!***********************************************************************
			\brief
				Constructor for the OpenGL Shader class
			\param name
				The name of the shader
			\param vertexSrc
				The source code of the vertex shader
			\param fragmentSrc
				The source code of the fragment shader
		*************************************************************************/
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		/*!***********************************************************************
			\brief
				Destructor for the OpenGL Shader class
		*************************************************************************/
		~OpenGLShader();

		uint32_t GetID() override;

		/*!***********************************************************************
			\brief
				Bind the shader
		*************************************************************************/
		void Bind() const override;

		/*!***********************************************************************
			\brief
				Unbind the shader
		*************************************************************************/
		void Unbind() const override;


		/*!***********************************************************************
			\brief
				Get the name of the shader
			\return
				The name of the shader
		*************************************************************************/
		const std::string& GetName() const override { return mName; }

		/*!***********************************************************************
			\brief
				Set the data of the shader: overloads for different types of data
			\param name
				The name of the data
			\param value
				The value of the data
		*************************************************************************/
		void Set(const char* name, const int& value) override;
		void Set(const char* name, const int* values, const uint32_t& count) override;
		void Set(const char* name, const float& value) override;
		void Set(const char* name, const glm::vec2& value) override;
		void Set(const char* name, const glm::vec3& value) override;
		void Set(const char* name, const glm::vec4& value) override;
		void Set(const char* name, const glm::mat3& value) override;
		void Set(const char* name, const glm::mat4& value) override;

	private:
		using GLenum = unsigned int;

		/*!***********************************************************************
			\brief
				Pushes the uniform into shader: overloads for different types of data
			\param name
				The name of the uniform
			\param value
				The value of the data
		*************************************************************************/
		void PushUniform(const char* name, const int& value);
		void PushUniform(const char* name, const int* values, const uint32_t count);
		void PushUniform(const char* name, const float& value);
		void PushUniform(const char* name, const glm::vec2& value);
		void PushUniform(const char* name, const glm::vec3& value);
		void PushUniform(const char* name, const glm::vec4& value);
		void PushUniform(const char* name, const glm::mat3& value);
		void PushUniform(const char* name, const glm::mat4& value);

		/*!***********************************************************************
			\brief
				Converts the string to the OpenGL Shader Type
			\param type
				The string of the shader type
			\return
				The OpenGL Shader Type
		*************************************************************************/
		GLenum ShaderTypeFromString(const std::string& type);

		/*!***********************************************************************
			\brief
				Reads the file from the file path
			\param filepath
				The file path to the shader file
			\return
				The string of the file
		*************************************************************************/
		std::string ReadFile(const std::string& filepath);

		/*!***********************************************************************
			\brief
				Preprocesses the shader source
			\param source
				The source of the shader
			\return
				The unordered map of the shader source
		*************************************************************************/
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);


		/*!***********************************************************************
			\brief
				Compiles the shader
			\param shaderSources
				The unordered map of the shader source
		*************************************************************************/
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		int GetUniformLocation(std::string const& uniformName);

		std::string mName; // Name of the shader
		unsigned int mRendererID; // ID of the shader
		std::unordered_map<std::string, int> mUniformLocations{};

	};

}
#endif