/******************************************************************************/
/*!
\file		Texture.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 8, 2024
\brief		Declares the Generic class for Texture primitives for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <string>
#include <Core/Core.hpp>
#include <Assets/Asset.hpp>
#include <Assets/AssetMetaData.hpp>

namespace Borealis
{
	enum class ImageFormat
	{
		None = 0,
		RGB8,
		RGBA8
	};

	struct TextureInfo
	{
		uint32_t width = 1;
		uint32_t height = 1;

		ImageFormat imageFormat = ImageFormat::RGBA8;
		bool generateMips = true;
	};

	// Base Class for Textures, Completely Virtual
	class Texture : public Asset
	{
	public:
		/*!***********************************************************************
			\brief
				Destructor of the Texture Virtual Class
		*************************************************************************/
		virtual ~Texture() {};

		/*!***********************************************************************
			\brief
				Get the Width of the Texture
			\return
				uint32_t - Width of the Texture
		*************************************************************************/
		virtual uint32_t GetWidth() const = 0;

		/*!***********************************************************************
			\brief
				Get the Height of the Texture
			\return
				uint32_t - Height of the Texture
		*************************************************************************/
		virtual uint32_t GetHeight() const = 0;

		/*!***********************************************************************
			\brief
				Get the Renderer ID of the Texture
			\return
				uint32_t - Renderer ID of the Texture
		*************************************************************************/
		virtual uint32_t GetRendererID() const = 0;

		/*!***********************************************************************
			\brief
				Set the Data of the Texture
			\param data
				void* - Data to be set
			\param size
				uint32_t - Size of the Data
		*************************************************************************/
		virtual void SetData(void* data, uint32_t size) = 0;

		/*!***********************************************************************
			\brief
				Bind the Texture to a Unit
			\param unit
				uint32_t - Unit to bind the Texture to
		*************************************************************************/
		virtual void Bind(uint32_t unit = 0) const = 0;

		/*!***********************************************************************
		\brief
			Get validity of texture
		\return
			bool - validity of texture
		*************************************************************************/
		virtual bool IsValid() const = 0;

		/*!***********************************************************************
			\brief
				Operator overload for comparing two Textures
			\param[in] other
				const Texture& - Texture to compare with
			\return
				bool - True if the Textures are the same, False otherwise
		*************************************************************************/
		virtual bool operator== (const Texture& other) const = 0;
	protected:
		virtual void SetRendererID(uint32_t id) = 0;
	}; // Class Texture

	// Virtual Class for 2D Textures
	class Texture2D : public Texture
	{
	public:
		/*!***********************************************************************
			\brief
				Create a texture2D based on the Graphics API
			\param[in] textureInfo
				const textureInfo& - Infomation of the Texture
		*************************************************************************/
		static Ref<Texture2D> Create(const TextureInfo& textureInfo);

		/*!***********************************************************************
			\brief
				Create a texture2D based on the Graphics API
			\param[in] path
				const std::string& - Path to the Texture
		*************************************************************************/
		static Ref<Texture2D> Create(const std::string& path, std::optional<TextureConfig> textureConfig = std::nullopt);

		static Ref<Texture2D> GetDefaultTexture();
		static void VerifyTexture(Ref<Texture2D> texture);


		static Ref<Asset> Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData);
		static void Reload(AssetMetaData const& assetMetaData, Ref<Asset> const&);

		//static void Reload(AssetMetaData const& assetMetaData);
		void swap(Asset& o) override;

	private:
		static Ref<Texture2D> mDefault;

	}; // Class Texture2D

	class TextureCubeMap : public Texture
	{
	public:
		static Ref<TextureCubeMap> Create(std::filesystem::path const& path);

		static Ref<TextureCubeMap> Load(AssetMetaData const& assetMetaData);

		static Ref<TextureCubeMap> GetDefaultCubeMap();

		static void SetDefaultCubeMap(AssetMetaData const& assetMetaData);

		static Ref<TextureCubeMap> GetDefaultCubeMap2();
		void swap(Asset& o) override;

	private:

		static Ref<TextureCubeMap> mDefaultCubeMap;
		static Ref<TextureCubeMap> mDefaultCubeMap2;
	};

} // Namespace Borealis

#endif