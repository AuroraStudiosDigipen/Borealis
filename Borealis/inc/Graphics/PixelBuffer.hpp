/******************************************************************************
/*!
\file       PixelBuffer.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 14, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef PixelBuffer_HPP
#define PixelBuffer_HPP

#include <Core/Core.hpp>

namespace Borealis
{
	enum class PixelBufferTextureFormat
	{
		None,
		RedInteger
	};

	struct PixelBufferTextureProperties
	{
		/*!***********************************************************************
			\brief
				Default constructor for PixelBufferTextureProperties
		*************************************************************************/
		PixelBufferTextureProperties() {};

		/*!***********************************************************************
			\brief
				Constructor for PixelBufferTextureProperties
			\param format
				The format of the texture
		*************************************************************************/
		PixelBufferTextureProperties(PixelBufferTextureFormat format)
			: mTextureFormat(format) {}
		PixelBufferTextureFormat mTextureFormat = PixelBufferTextureFormat::None; /*!< The format of the texture */
	}; // Struct FrameBufferTextureProperties

	struct PixelBufferProperties
	{
		uint32_t Width, Height;	 /*!< The width and height of the PixelBuffer */
		PixelBufferTextureFormat TextureFormat; //unused rn
	}; // Struct PixelBufferProperties

	class PixelBuffer
	{
	public:

		virtual ~PixelBuffer() = default;

		virtual void ReadTexture(uint32_t index) = 0;

		virtual int ReadPixel(int x, int y) = 0;
		virtual void Bind() = 0;
		
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const PixelBufferProperties& GetProperties() const = 0;

		static Ref<PixelBuffer> Create(const PixelBufferProperties& properties);
	};
}

#endif
