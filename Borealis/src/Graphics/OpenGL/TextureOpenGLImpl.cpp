/******************************************************************************/
/*!
\file		TextureOpenGLImpl.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Declares the OpenGL classes for Textures

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include <BorealisPCH.hpp>
#include <Graphics/OpenGL/TextureOpenGLImpl.hpp>
#include <Core/LoggerSystem.hpp>
#include <Assets/AssetManager.hpp>
#include <gli.hpp>
namespace Borealis
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : mPath(path)
	{
		PROFILE_FUNCTION();

		gli::texture Texture;
		if (AssetManager::IsPakLoaded())
		{
			// concat to the last path
			try
			{
				std::string subPath = path.substr(path.find_last_of("/\\") + 1);
				uint64_t id = std::stoull(subPath);
				char* buffer;
				uint64_t size;
				AssetManager::RetrieveFromPak(id, buffer, size);
				Texture = gli::load(buffer, size);
				delete[] buffer;
			}
			catch (...)
			{
				Texture = gli::load(path);
			}
		}
		else
		{
			Texture = gli::load(path);
		}
		if (Texture.empty())
		{
			BOREALIS_CORE_ASSERT(false,"Invalid texture file {}");
			mValid = false;
			return;
		}


		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		mInternalFormat = Format.Internal;

		switch (Format.Internal)
		{
		case GL_RGB:  mInternalFormat = GL_SRGB8; break;
		case GL_RGBA: mInternalFormat = GL_SRGB8_ALPHA8; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:	  
					  mInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		default: break;
		}

		mDataFormat = Format.External;
		switch (Format.Internal) 
		{
			case GL_RED: mChannels = 1; break;
			case GL_RG: mChannels = 2; break;
			case GL_RGB: mChannels = 3; break;
			case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			case GL_RGBA: mChannels = 4; break;
			default: mChannels = 0;
		}


		GLenum Target = GL.translate(Texture.target());
		assert(gli::is_compressed(Texture.format()) && Texture.target() == gli::TARGET_2D);

		glm::tvec3<GLsizei> const Extent(Texture.extent());

		mWidth = Extent.x;
		mHeight = Extent.y;

		glGenTextures(1, &mRendererID);
		glBindTexture(Target, mRendererID);
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);

		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glm::tvec3<GLsizei> Extent = Texture.extent(Level);

			glCompressedTexImage2D(
				GL_TEXTURE_2D,
				static_cast<GLint>(Level),
				mInternalFormat,
				Extent.x, Extent.y,
				0,
				static_cast<GLsizei>(Texture.size(Level)),
				Texture.data(0, 0, Level));
		}

		glBindTexture(Target, 0);

		mValid = true;
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, std::optional<TextureConfig> textureConfig)
	{
		PROFILE_FUNCTION();
		gli::texture Texture;
		if (AssetManager::IsPakLoaded())
		{
			// concat to the last path
			try
			{
				std::string subPath = path.substr(path.find_last_of("/\\") + 1);
				uint64_t id = std::stoull(subPath);
				char* buffer;
				uint64_t size;
				AssetManager::RetrieveFromPak(id, buffer, size);
				Texture = gli::load(buffer, size);
				delete[] buffer;
			}
			catch(...)
			{
				Texture = gli::load(path);
			}
		}
		else
		{
			Texture = gli::load(path);
		}
		if (Texture.empty())
		{
			BOREALIS_CORE_ASSERT(false, "Invalid texture file {}");
			mValid = false;
			return;
		}


		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		TextureConfig config{};
		if (textureConfig.has_value())
			config = textureConfig.value();

		mInternalFormat = Format.Internal;

		switch (Format.Internal)
		{
		case GL_RGB:  mInternalFormat = GL_SRGB8; break;
		case GL_RGBA: mInternalFormat = GL_SRGB8_ALPHA8; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:	  
			if(config.sRGB)
					  mInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			else
					  mInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default: break;
		}

		mDataFormat = Format.External;
		switch (Format.Internal)
		{
		case GL_RED: mChannels = 1; break;
		case GL_RG: mChannels = 2; break;
		case GL_RGB: mChannels = 3; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_RGBA: mChannels = 4; break;
		default: mChannels = 0;
		}


		GLenum Target = GL.translate(Texture.target());
		assert(gli::is_compressed(Texture.format()) && Texture.target() == gli::TARGET_2D);

		glm::tvec3<GLsizei> const Extent(Texture.extent());

		mWidth = Extent.x;
		mHeight = Extent.y;

		glGenTextures(1, &mRendererID);
		glBindTexture(Target, mRendererID);
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);



		GLuint linearMode{};
		GLuint linearModeMag{};
		switch (config.filterMode)
		{
		case TextureFilter::_LINEAR:
			if (config.generateMipMaps)
				linearMode = GL_LINEAR_MIPMAP_LINEAR;
			else
				linearMode = GL_LINEAR;
				linearModeMag = GL_LINEAR;
			break;
		case TextureFilter::_NEAREST:
			if (config.generateMipMaps)
				linearMode = GL_NEAREST_MIPMAP_NEAREST;
			else
				linearMode = GL_NEAREST;
				linearModeMag = GL_NEAREST;
			break;
		default:
			break;
		}

		GLuint wrapMode{};

		switch (config.wrapMode)
		{
		case TextureWrap::_REPEAT:
			wrapMode = GL_REPEAT;
			break;
		case TextureWrap::_MIRRORED:
			wrapMode = GL_MIRRORED_REPEAT;
			break;
		case TextureWrap::_CLAMP_TO_EDGE:
			wrapMode = GL_CLAMP_TO_EDGE;
			break;
		case TextureWrap::_CLAMP_TO_BORDER:
			wrapMode = GL_CLAMP_TO_BORDER;
			break;
		default:
			break;
		}

		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, linearMode);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, linearModeMag);


		glTexParameteri(Target, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, wrapMode);

		for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glm::tvec3<GLsizei> Extent = Texture.extent(Level);

			glCompressedTexImage2D(
				GL_TEXTURE_2D,
				static_cast<GLint>(Level),
				mInternalFormat,
				Extent.x, Extent.y,
				0,
				static_cast<GLsizei>(Texture.size(Level)),
				Texture.data(0, 0, Level));
		}

		glBindTexture(Target, 0);

		mValid = true;

		BOREALIS_CORE_ASSERT(glGetError() == GL_NO_ERROR, "Error");
	}

	static GLenum ImageFormatToGLDataFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB;
		case ImageFormat::RGBA8: return GL_RGBA;
		}

		BOREALIS_CORE_ASSERT(false);
		return 0;
	}

	static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB8;
		case ImageFormat::RGBA8: return GL_RGBA8;
		}

		BOREALIS_CORE_ASSERT(false);
		return 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureInfo& textureInfo) : mWidth(textureInfo.width), mHeight(textureInfo.height)
	{
		PROFILE_FUNCTION();

		mInternalFormat = ImageFormatToGLInternalFormat(textureInfo.imageFormat);
		mDataFormat = ImageFormatToGLDataFormat(textureInfo.imageFormat);

		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mDataFormat, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);

		mValid = true;
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		PROFILE_FUNCTION();

		glDeleteTextures(1, &mRendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		PROFILE_FUNCTION();

		uint32_t bpp = mDataFormat == GL_RGBA ? 4 : 3;

		BOREALIS_CORE_ASSERT(size == mWidth * mHeight * bpp, "Data must be entire texture!");
		
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::CheckTexture(Ref<Texture2D> texture)
	{
		auto rendererID = texture->GetRendererID();
		if (rendererID == 0)
		{
			BOREALIS_CORE_ERROR("Texture is not valid!");
			return;
		}

		BOREALIS_CORE_INFO("RendererID: {}", texture->GetRendererID());
		int width, height, format;
		texture->Bind();
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		BOREALIS_CORE_INFO("Width: {}, Height: {}, Format: {}", width, height, format);
	}

	void OpenGLTexture2D::OpenGLTexture2D::Bind(uint32_t unit) const
	{
		PROFILE_FUNCTION();

		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
	}
	bool OpenGLTexture2D::IsValid() const
	{
		return mValid;
	}

	void OpenGLTexture2D::swap(Asset& o)
	{
		OpenGLTexture2D& other = dynamic_cast<OpenGLTexture2D&>(o);
		std::swap(mRendererID, other.mRendererID);
		std::swap(mWidth, other.mWidth);
		std::swap(mHeight, other.mHeight);
		std::swap(mInternalFormat, other.mInternalFormat);
		std::swap(mDataFormat, other.mDataFormat);
		std::swap(mChannels, other.mChannels);
		std::swap(mValid, other.mValid);
		std::swap(mPath, other.mPath);
	}


	OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		// Load the texture using gli
		gli::texture Texture;
		if (AssetManager::IsPakLoaded())
		{
			try
			{
				// concat to the last path
				std::string subPath = path.filename().string();
				uint64_t id = std::stoull(subPath);
				char* buffer;
				uint64_t size;
				AssetManager::RetrieveFromPak(id, buffer, size);
				Texture = gli::load(buffer, size);
				delete[] buffer;
			}
			catch (...)
			{
				Texture = gli::load(path.string());
			}
		}
		else
		{
			Texture = gli::load(path.string());
		}

		if (Texture.empty())
		{
			BOREALIS_CORE_ASSERT(false, "Invalid texture file {}");
			mValid = false;
			return;
		}

		// Verify that the texture is a cube map
		if (Texture.target() != gli::TARGET_CUBE)
		{
			BOREALIS_CORE_ASSERT(false, "Texture is not a cube map!");
			mValid = false;
			return;
		}

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		mInternalFormat = Format.Internal;
		mDataFormat = Format.External;

		mInternalFormat = Format.Internal;

		switch (Format.Internal)
		{
		case GL_RGB:  mInternalFormat = GL_SRGB8; break;
		case GL_RGBA: mInternalFormat = GL_SRGB8_ALPHA8; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			mInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			break;
		default: break;
		}

		switch (Format.Internal)
		{
		case GL_RED: mChannels = 1; break;
		case GL_RG: mChannels = 2; break;
		case GL_RGB: mChannels = 3; break;
		case GL_RGBA: mChannels = 4; break;
		default: mChannels = 0;
		}

		GLenum Target = GL.translate(Texture.target());
		glm::tvec3<GLsizei> const Extent(Texture.extent(0));

		mWidth = Extent.x;
		mHeight = Extent.y;

		glGenTextures(1, &mRendererID);
		glBindTexture(Target, mRendererID);

		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (std::size_t Face = 0; Face < 6; ++Face)
		{
			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glm::tvec3<GLsizei> Extent = Texture.extent(Level);
				glCompressedTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)Face,
					static_cast<GLint>(Level),
					mInternalFormat,
					Extent.x, Extent.y,
					0,
					static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(0, Face, Level));
			}
		}

		glBindTexture(Target, 0);

		mValid = true;
	}
	OpenGLTextureCubeMap::~OpenGLTextureCubeMap()
	{
		PROFILE_FUNCTION();

		glDeleteTextures(1, &mRendererID);
	}

	void OpenGLTextureCubeMap::SetData(void* data, uint32_t size)
	{
	}

	void OpenGLTextureCubeMap::Bind(uint32_t unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mRendererID);
		int err = glGetError();
		if (err != GL_NO_ERROR)
		{
			BOREALIS_CORE_INFO("Error at cubemap {}", err);
		}
	}

	bool OpenGLTextureCubeMap::IsValid() const
	{
		return mValid;
	}


}

