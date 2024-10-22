/******************************************************************************/
/*!
\file		Texture.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Declares the Generic class for Textures

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include <BorealisPCH.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/OpenGL/TextureOpenGLImpl.hpp>
namespace Borealis
{
	Ref<Texture2D> Texture2D::mDefault = nullptr;
	Ref<Texture2D> Texture2D::Create(const TextureInfo& textureInfo)
	{
		Ref<Texture2D> texture = nullptr;
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported");
			case RendererAPI::API::OpenGL: 
				texture = MakeRef<OpenGLTexture2D>(textureInfo);
				if (!texture->IsValid())
				{
					texture = nullptr;
				}
				break;
		}
		if (!texture)
		{
			BOREALIS_CORE_ASSERT(false, "Unknown RendererAPI");
			texture = GetDefaultTexture();
		}
		return texture;

		return Ref<Texture2D>();
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		Ref<Texture2D> texture = nullptr;
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); break;
		case RendererAPI::API::OpenGL: 
			texture = MakeRef<OpenGLTexture2D>(path);
			if (!texture->IsValid())
			{
				texture = nullptr;
			}
			break;
		}
		if(!texture)
		{
			BOREALIS_CORE_ASSERT(false, "Unknown RendererAPI");
			texture = GetDefaultTexture();
		}
		return texture;

		return Ref<Texture2D>();
	}
	Ref<Texture2D> Texture2D::GetDefaultTexture()
	{
		if (!mDefault)
		{
			mDefault = Create("Resources/textures/missing_texture.DDS");
		}
		return mDefault;
	}
}
