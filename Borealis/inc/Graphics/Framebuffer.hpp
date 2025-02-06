/******************************************************************************/
/*!
\file		Framebuffer.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 11, 2024
\brief		Declares the Generic class for Frame buffers

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP
#include <Core/Core.hpp>
namespace Borealis
{
	enum class FramebufferTextureFormat
	{
		None,

		//Color
		RGB16F,
		RGBA8,
		RGBA16F,
		RedInteger,
		R16F,

		//Stencil
		Depth24Stencil8,
		DepthArray,

		//Default
		Depth = Depth24Stencil8
	};

	struct FrameBufferTextureProperties
	{
		/*!***********************************************************************
			\brief
				Default constructor for FrameBufferTextureProperties
		*************************************************************************/
		FrameBufferTextureProperties() {};

		/*!***********************************************************************
			\brief
				Constructor for FrameBufferTextureProperties
			\param format
				The format of the texture
		*************************************************************************/
		FrameBufferTextureProperties(FramebufferTextureFormat format)
			: mTextureFormat(format) {}
		FramebufferTextureFormat mTextureFormat = FramebufferTextureFormat::None; /*!< The format of the texture */
	}; // Struct FrameBufferTextureProperties

	struct FrameBufferAttachmentProperties
	{
		FrameBufferAttachmentProperties() = default;

		/*!***********************************************************************
			\brief
				Constructor for FrameBufferAttachmentProperties
			\param[in] attachments
				The list of FrameBufferTextureProperties
		*************************************************************************/
		FrameBufferAttachmentProperties(std::initializer_list<FrameBufferTextureProperties> attachments)
			: mAttachments(attachments) {}
		std::vector<FrameBufferTextureProperties> mAttachments; /*!< The list of FrameBufferTextureProperties */
	}; // Struct FrameBufferAttachmentProperties

	struct FrameBufferProperties
	{
		uint32_t Width, Height;	 /*!< The width and height of the FrameBuffer */
		uint32_t Samples = 1; /*!< The number of samples */
		bool SwapChainTarget = false; /*!< Whether the FrameBuffer is a swap chain target */
		FrameBufferAttachmentProperties Attachments; /*!< The FrameBufferAttachmentProperties */
	}; // Struct FrameBufferProperties

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		/*!***********************************************************************
			\brief
				Bind the FrameBuffer
		*************************************************************************/
		virtual void Bind() = 0;

		/*!***********************************************************************
			\brief
				Unbind the FrameBuffer
		*************************************************************************/
		virtual void Unbind() = 0;

		virtual void BindTexture(uint32_t attachmentIndex, uint32_t textureUnit) = 0;

		virtual void BindDepthBuffer(uint32_t textureUnit, bool is3D = false) = 0;

		/*!***********************************************************************
			\brief
				Resize the Frame Buffer
			\param[in] width
				The width of the FrameBuffer
			\param[in] height
				The height of the FrameBuffer
		*************************************************************************/
		virtual void Resize(uint32_t width, uint32_t height) = 0;

	/*!***********************************************************************
			\brief
				Read the pixel at the specified x and y coordinates
			\param[in] attachmentIndex
				The index of the attachment
			\param[in] x
				The x coordinate
			\param[in] y
				The y coordinate
			\return
				The pixel value
		*************************************************************************/
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		/*!***********************************************************************
			\brief
				Clear the attachment at the specified index
			\param[in] attachmentIndex
				The index of the attachment
			\param[in] value
				The value to clear the attachment to
		*************************************************************************/
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, glm::vec4 value) = 0;

		/*!***********************************************************************
			\brief
				Get the color attachment renderer ID
			\param[in] index
				The index of the color attachment
			\return
				The color attachment renderer ID
		*************************************************************************/
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual uint32_t DetachDepthBuffer() = 0;

		virtual void AttachDepthBuffer(uint32_t index) = 0;

		/*!***********************************************************************
			\brief
				Get the Frame Buffer Properties
			\return
				The Frame Buffer Properties
		*************************************************************************/
		virtual const FrameBufferProperties& GetProperties() const = 0;

		/*!***********************************************************************
			\brief
				Create a Frame Buffer depending on the Graphics API
			\param[in] properties		
				The FrameBufferProperties
		*************************************************************************/
		static Ref<FrameBuffer> Create(const FrameBufferProperties& properties);

	}; // Class FrameBuffer
} // Namespace Borealis

#endif