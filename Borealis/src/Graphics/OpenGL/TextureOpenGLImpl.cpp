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

#include <stb_image.h>
#include <gli.hpp>
namespace Borealis
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : mPath(path)
	{
		PROFILE_FUNCTION();

		gli::texture Texture = gli::load(path);
		if (Texture.empty())
		{
			BOREALIS_CORE_ASSERT(false,"Invalid texture file {}");
			mValid = false;
			return;
		}


		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		mInternalFormat = Format.Internal;
		mDataFormat = Format.External;
		switch (Format.Internal) 
		{
			case GL_RED: mChannels = 1; break;
			case GL_RG: mChannels = 2; break;
			case GL_RGB: mChannels = 3; break;
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

		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glm::tvec3<GLsizei> Extent = Texture.extent(Level);

			glCompressedTexImage2D(
				GL_TEXTURE_2D,
				static_cast<GLint>(Level),
				Format.Internal,
				Extent.x, Extent.y,
				0,
				static_cast<GLsizei>(Texture.size(Level)),
				Texture.data(0, 0, Level));
		}

		glBindTexture(Target, 0);

		mValid = true;

		//int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		//stbi_uc* data = nullptr;
		//{
		//	PROFILE_SCOPE("stbi_load : OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : mPath(path)");
		//	data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		//}
		//if (!data)
		//{
		//	BOREALIS_CORE_ERROR("Failed to load image: {}", path);
		//}
		//mWidth = width;
		//mHeight = height;
		//mChannels = channels;

		//if (channels == 4) // RGBA
		//{
		//	mInternalFormat = GL_RGBA8;
		//	mDataFormat = GL_RGBA;
		//}
		//else if (channels == 3) // RGB
		//{
		//	mInternalFormat = GL_RGB8;
		//	mDataFormat = GL_RGB;
		//}
		//else
		//{
		//	BOREALIS_CORE_ERROR("Unsupported image format: {}", path);
		//}


		//glGenTextures(1, &mRendererID);
		//glBindTexture(GL_TEXTURE_2D, mRendererID);
		//glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mDataFormat, GL_UNSIGNED_BYTE, nullptr);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//
		//stbi_image_free(data);


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
}

