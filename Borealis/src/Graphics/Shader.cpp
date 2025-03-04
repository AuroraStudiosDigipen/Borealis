/******************************************************************************/
/*!
\file		Shader.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 6, 2024
\brief		Implements the functions for Generic Shader Class of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Core/LoggerSystem.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/OpenGL/ShaderOpenGLImpl.hpp>
#include <Graphics/UniformBufferObject.hpp>
#include <Graphics/OpenGL/UniformBufferObjectOpenGLImpl.hpp>
#include <Graphics/UBOBindings.hpp>

#

namespace Borealis
{
	//default 3d material shader
	static Ref<Shader> s_material3dShader = nullptr;
	Ref<Shader> Shader::GetDefault3DMaterialShader()
	{
		if (!s_material3dShader)
		{
			s_material3dShader = Create("engineResources/Shaders/Renderer3D_Material.glsl");
			UniformBufferObject::BindToShader(s_material3dShader->GetID(), "Camera", CAMERA_BIND);
			UniformBufferObject::BindToShader(s_material3dShader->GetID(), "MaterialUBO", MATERIAL_ARRAY_BIND);
			UniformBufferObject::BindToShader(s_material3dShader->GetID(), "LightsUBO", LIGHTING_BIND);
			UniformBufferObject::BindToShader(s_material3dShader->GetID(), "AnimationUBO", ANIMATION_BIND);

			BOREALIS_CORE_INFO("Animation UBO size : {} bytes", UniformBufferObjectOpenGLImpl::GetBlockSize(s_material3dShader->GetID(), "AnimationUBO"));
		}
		return s_material3dShader;
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return MakeRef<OpenGLShader>(filepath);
		}
		BOREALIS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return MakeRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		BOREALIS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	
	void ShaderAPI::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		if (hasShader(name))
		{
			BOREALIS_CORE_WARN("Shader {} already exists!", shader->GetName());
		}
		mShaders[name] = shader;

	}
	void ShaderAPI::Add(const Ref<Shader>& shader, const std::string& name)
	{
		if (hasShader(name))
		{
			BOREALIS_CORE_WARN("Shader {} already exists!", shader->GetName());
		}
		mShaders[name] = shader;
	}
	Ref<Shader> ShaderAPI::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	Ref<Shader> ShaderAPI::Load(const std::string& filepath, const std::string& name)
	{
		auto shader = Shader::Create(filepath);
		Add(shader, name);
		return shader;
	}
	Ref<Shader> ShaderAPI::Get(const std::string& name)
	{
		if (!hasShader(name))
		{
			BOREALIS_CORE_ASSERT(false, "Shader name not found in library!");
			return nullptr;
		}
		return mShaders[name];
	}
	bool ShaderAPI::hasShader(const std::string& name) const
	{
		return mShaders.find(name) != mShaders.end();
	}
}