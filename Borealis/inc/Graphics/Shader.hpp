/******************************************************************************/
/*!
\file		Shader.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 6, 2024
\brief		Declares the functions for Generic Shader Class of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>
#include <Core/Core.hpp>

namespace Borealis
{
	class Shader
	{
	public:

		/*!***********************************************************************
			\brief
				Destructor of the Shader Virtual Class
		*************************************************************************/
		virtual ~Shader() {};

		virtual uint32_t GetID() = 0;

		/*!***********************************************************************
			\brief
				Bind the Shader
		*************************************************************************/
		virtual void Bind() const = 0;

		/*!***********************************************************************
			\brief
				Unbind the Shader
		*************************************************************************/
		virtual void Unbind() const = 0;

		/*!***********************************************************************
			\brief
				Get the Name of the Shader
			\return
			    The Name of the Shader
		*************************************************************************/
		virtual const std::string& GetName() const = 0;

		/*!***********************************************************************
			\brief
				Create the Shader in the corresponding Graphics API
			\param[in] filepath
				The Filepath of the Shader
		*************************************************************************/
		static Ref<Shader> Create(const std::string& filepath);

		/*!***********************************************************************
			\brief
				Create the Shader in the corresponding Graphics API
			\param[in] name
				The Name of the Shader
			\param[in] vertexSrc
				Src code of the Vertex Shader
			\param[in] fragmentSrc
				Src code of the Fragment Shader
		*************************************************************************/
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Ref<Shader> GetDefault3DMaterialShader();

		/*!***********************************************************************
			\brief
				Set the Uniforms of the Shader: Different overloads for different
				data type
			\param[in] name
				The Name of the Uniform
			\param[in] value
				The Value of the Uniform
		*************************************************************************/
		virtual void Set(const char* name, const int& value) = 0;
		virtual void Set(const char* name, const int* values, const uint32_t& count) = 0;
		virtual void Set(const char* name, const float& value) = 0;
		virtual void Set(const char* name, const glm::vec2& value) = 0;
		virtual void Set(const char* name, const glm::vec3& value) = 0;
		virtual void Set(const char* name, const glm::vec4& value) = 0;
		virtual void Set(const char* name, const glm::mat3& value) = 0;
		virtual void Set(const char* name, const glm::mat4& value) = 0;
	}; // class Shader

	class ShaderAPI
	{
	public:

		/*!***********************************************************************
			\brief
				Add the shader to the map of shaders
		*************************************************************************/
		void Add(const Ref<Shader>& shader);

		/*!***********************************************************************
			\brief
				Add the shader to the map of shaders
			\param[in] name
				The Name of the Shader
		*************************************************************************/
		void Add(const Ref<Shader>& shader, const std::string& name);

		/*!***********************************************************************
			\brief
				Load the shader from the filepath
			\param[in] filepath
				The Filepath of the Shader
			\return
				The Shader
		*************************************************************************/
		Ref<Shader> Load(const std::string& filepath);

		/*!***********************************************************************
			\brief
				Load the shader from the filepath
			\param[in] name
				The Name of the Shader
			\param[in] filepath
				The Filepath of the Shader
			\return
				The Shader
		*************************************************************************/
		Ref<Shader> Load(const std::string& filepath, const std::string& name);

		/*!***********************************************************************
			\brief
				Get the Shader from the map of shaders
			\param[in] name
				The Name of the Shader
			\return
				The Shader
		*************************************************************************/
		Ref<Shader> Get(const std::string& name);

		/*!***********************************************************************
			\brief
				Check if the Shader exists in the map of shaders
			\param[in] name
				The Name of the Shader
			\return
				True if the Shader exists, False otherwise
		*************************************************************************/
		bool hasShader(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> mShaders; //!< Map of Shaders
	}; // class ShaderAPI

} // namespace Borealis
#endif