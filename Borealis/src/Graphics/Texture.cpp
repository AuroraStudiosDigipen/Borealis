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

	Ref<TextureCubeMap> TextureCubeMap::mDefaultCubeMap = nullptr;
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
	Ref<Texture2D> Texture2D::Create(const std::string& path, std::optional<TextureConfig> textureConfig)
	{
		Ref<Texture2D> texture = nullptr;
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); break;
		case RendererAPI::API::OpenGL: 
			texture = MakeRef<OpenGLTexture2D>(path, textureConfig);
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
			mDefault = Create("engineResources/textures/particles.DDS");
		}
		return mDefault;
	}

	void Texture2D::VerifyTexture(Ref<Texture2D> texture)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); break;
		case RendererAPI::API::OpenGL:
			OpenGLTexture2D::CheckTexture(texture);
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
	}


	Ref<Asset> Texture2D::Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData)
	{
		return Create((cachePath / std::to_string(assetMetaData.Handle)).string(), GetConfig<TextureConfig>(assetMetaData.Config));
	}

	void Texture2D::Reload(AssetMetaData const& assetMetaData, Ref<Asset> const& asset)
	{
		Ref<Texture2D> newAsset = Create(assetMetaData.CachePath.string(), GetConfig<TextureConfig>(assetMetaData.Config));
		newAsset->mAssetHandle = assetMetaData.Handle;

		newAsset->swap(*asset);
	}

	void Texture2D::swap(Asset& o)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported");
		case RendererAPI::API::OpenGL:
			OpenGLTexture2D& original = dynamic_cast<OpenGLTexture2D&>(*this);
			OpenGLTexture2D& other = dynamic_cast<OpenGLTexture2D&>(o);
			original.swap(other);
			break;
		}
	}

	//void Texture2D::Reload(AssetMetaData const& assetMetaData)
	//{
	//	TextureConfig config = GetConfig<TextureConfig>(assetMetaData.Config);
	//	creat
	//}

	Ref<TextureCubeMap> TextureCubeMap::Create(std::filesystem::path const& path)
	{
		Ref<TextureCubeMap> texture = nullptr;
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); break;
		case RendererAPI::API::OpenGL:
			texture = MakeRef<OpenGLTextureCubeMap>(path);
			if (!texture->IsValid())
			{
				texture = nullptr;
			}
			break;
		}
		return texture;

		return Ref<TextureCubeMap>();
	}

	Ref<TextureCubeMap> TextureCubeMap::Load(AssetMetaData const& assetMetaData)
	{
		return Create(assetMetaData.CachePath);
	}

	Ref<TextureCubeMap> TextureCubeMap::GetDefaultCubeMap()
	{
		if (!mDefaultCubeMap)
		{
			mDefaultCubeMap = Create(std::filesystem::path("engineResources/Textures/SkyBoxPng4k.dds"));
		}
		return mDefaultCubeMap;
	}

	void TextureCubeMap::SetDefaultCubeMap(AssetMetaData const& assetMetaData)
	{
		mDefaultCubeMap = Load(assetMetaData);
	}

	Ref<TextureCubeMap> TextureCubeMap::GetDefaultCubeMap2()
	{
		if (!mDefaultCubeMap)
		{
			mDefaultCubeMap = Create(std::filesystem::path("engineResources/Textures/SkyBoxPng4k.dds"));
		}
		return mDefaultCubeMap;
	}
	void TextureCubeMap::swap(Asset& o)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BOREALIS_CORE_ASSERT(false, "RendererAPI::None is not supported"); break;
		case RendererAPI::API::OpenGL:
			TextureCubeMap& self = dynamic_cast<TextureCubeMap&>(*this);
			TextureCubeMap& other = dynamic_cast<TextureCubeMap&>(o);
			self.swap(other);
			break;
		}
	}
}
