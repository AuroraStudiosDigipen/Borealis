/******************************************************************************/
/*!
\file		TextureOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Declares the OpenGL classes for Textures

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef TEXTURE_OPENGL_IMPL_HPP
#define TEXTURE_OPENGL_IMPL_HPP
#include <glad/glad.h>
#include <Graphics/Texture.hpp>


namespace Borealis
{
	// Virtual Class for 2D Textures
	class OpenGLTexture2D : public Texture2D
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for OpenGL Texture2D
			\param[in] path
				Path to the texture file
		*************************************************************************/
		OpenGLTexture2D(const std::string& path);

		/*!***********************************************************************
			\brief
				Constructor for OpenGL Texture2D
			\param[in] path
				Path to the texture file
		*************************************************************************/
		OpenGLTexture2D(const std::string& path, std::optional<TextureConfig> textureConfig = std::nullopt);

		/*!***********************************************************************
			\brief
				Constructor for OpenGL Texture2D
			\param[in] textureInfo
				Infomation of the texture
		*************************************************************************/
		OpenGLTexture2D(const TextureInfo& textureInfo);

		/*!***********************************************************************
			\brief
				Destructor for OpenGL Texture2D
		*************************************************************************/
		~OpenGLTexture2D();

		/*!***********************************************************************
			\brief
				Set Data of a texture
			\param[in] data
				Pointer to the data
			\param[in] size
				Size of the data
		*************************************************************************/
		void SetData(void* data, uint32_t size) override;

		static void CheckTexture(Ref<Texture2D> texture);

		/*!***********************************************************************
			\brief
				Get Width of the texture
			\return
				Width of the texture
		*************************************************************************/
		uint32_t GetWidth() const override { return mWidth; }

		/*!***********************************************************************
			\brief
				Get Height of the texture
			\return
				Height of the texture
		*************************************************************************/
		uint32_t GetHeight() const override { return mHeight; }

		/*!***********************************************************************
			\brief
				Get Renderer ID of the texture
			\return
				Renderer ID of the texture
		*************************************************************************/
		uint32_t GetRendererID() const override { return mRendererID; }

		void SetRendererID(uint32_t ID) override {mRendererID = ID;}

		/*!***********************************************************************
			\brief
				Bind the texture
			\param[in] unit
				Texture unit to bind the texture to
		*************************************************************************/
		void Bind(uint32_t unit = 0) const override;

		bool IsValid() const override;

		void swap(Asset& other) override;


		/*!***********************************************************************
			\brief
				Operator overload to check if two textures are the same
			\param[in] other
				Other texture to compare with
			\return
				True if the textures are the same, false otherwise
		*************************************************************************/
		bool operator==(const Texture& other) const override{return mRendererID == ((OpenGLTexture2D&)other).mRendererID;}
	private:
		bool mValid;
		std::string mPath; // Path to the texture file
		uint32_t mRendererID; // Renderer ID of the texture
		uint32_t mWidth, mHeight, mChannels; // Width, Height and Channels of the texture
		GLenum mInternalFormat, mDataFormat; // Internal Format and Data Format of the texture
	};

	class OpenGLTextureCubeMap : public TextureCubeMap
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for OpenGL Texture2D
			\param[in] path
				Path to the texture file
		*************************************************************************/
		OpenGLTextureCubeMap(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Destructor for OpenGL Texture2D
		*************************************************************************/
		~OpenGLTextureCubeMap();

		/*!***********************************************************************
			\brief
				Set Data of a texture
			\param[in] data
				Pointer to the data
			\param[in] size
				Size of the data
		*************************************************************************/
		void SetData(void* data, uint32_t size) override;

		/*!***********************************************************************
			\brief
				Get Width of the texture
			\return
				Width of the texture
		*************************************************************************/
		uint32_t GetWidth() const override { return mWidth; }

		/*!***********************************************************************
			\brief
				Get Height of the texture
			\return
				Height of the texture
		*************************************************************************/
		uint32_t GetHeight() const override { return mHeight; }

		/*!***********************************************************************
			\brief
				Get Renderer ID of the texture
			\return
				Renderer ID of the texture
		*************************************************************************/
		uint32_t GetRendererID() const override { return mRendererID; }

		void SetRendererID(uint32_t id) override{mRendererID = id;}

		/*!***********************************************************************
			\brief
				Bind the texture
			\param[in] unit
				Texture unit to bind the texture to
		*************************************************************************/
		void Bind(uint32_t unit = 0) const override;

		bool IsValid() const override;

		/*!***********************************************************************
			\brief
				Operator overload to check if two textures are the same
			\param[in] other
				Other texture to compare with
			\return
				True if the textures are the same, false otherwise
		*************************************************************************/
		bool operator==(const Texture& other) const override { return mRendererID == ((OpenGLTextureCubeMap&)other).mRendererID; }
	private:
		bool mValid;
		std::string mPath; // Path to the texture file
		uint32_t mRendererID; // Renderer ID of the texture
		uint32_t mWidth, mHeight, mChannels; // Width, Height and Channels of the texture
		GLenum mInternalFormat, mDataFormat; // Internal Format and Data Format of the texture
	};
}

#endif