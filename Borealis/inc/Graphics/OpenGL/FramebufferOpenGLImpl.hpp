/******************************************************************************/
/*!
\file		FramebufferOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 11, 2024
\brief		Declares the OpenGL class for Frame buffers

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef FRAME_BUFFER_OPENGL_IMPL_HPP
#define FRAME_BUFFER_OPENGL_IMPL_HPP
#include <Core/Core.hpp>
#include <Graphics/Framebuffer.hpp>
namespace Borealis
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor of the OpenGL Frame Buffer
			\param props
				Properties of the Frame Buffer
		*************************************************************************/
		OpenGLFrameBuffer(const FrameBufferProperties& props);

		/*!***********************************************************************
			\brief
				Destructor of the OpenGL Frame Buffer
		*************************************************************************/
		~OpenGLFrameBuffer();

		/*!***********************************************************************
			\brief
				Bind the Frame Buffer
		*************************************************************************/
		void Bind() override;

		/*!***********************************************************************
			\brief
				Unbind the Frame Buffer
		*************************************************************************/
		void Unbind() override;

		void BindTexture(uint32_t attachmentIndex, uint32_t textureUnit) override;

		/*!***********************************************************************
			\brief
				Resize the Frame Buffer
			\param width
				Width of the Frame Buffer
			\param height
				Height of the Frame Buffer
		*************************************************************************/
		void Resize(uint32_t width, uint32_t height) override;

		/*!***********************************************************************
			\brief
				Read the pixel data from the frame buffer at a specific point
				as an int data
			\param[in] attachmentIndex
				Index of the attachment to read from
			\param[in] x
				X coordinate of the pixel
			\param[in] y
				Y coordinate of the pixel
			\return
				The pixel data as an int
		*************************************************************************/
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		/*!***********************************************************************
			\brief
				Clear the attachment at the specified index with a specific value
			\param[in] attachmentIndex
				Index of the attachment to clear
			\param[in] value
				Value to clear the attachment with
		*************************************************************************/
		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		/*!***********************************************************************
			\brief
				Get the properties of the Frame Buffer
			\return
				Properties of the Frame Buffer
		*************************************************************************/
		const FrameBufferProperties& GetProperties() const override { return mProps; }

		/*!***********************************************************************
			\brief
				Get the color attachment renderer ID
			\param index
				Index of the color attachment
			\return
				Renderer ID of the color attachment
		*************************************************************************/
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return mColorAttachments[index]; }

		/*!***********************************************************************
			\brief
				Recreates the Attachments of the FrameBuffer
		*************************************************************************/
		void Recreate();
	private:
		FrameBufferProperties mProps; //!< Properties of the Frame Buffer
		uint32_t mRendererID; //!< Renderer ID of the Frame Buffer

		std::vector<FrameBufferTextureProperties> mColorAttachmentProps; //!< Properties of the Color Attachments
		FrameBufferTextureProperties mDepthAttachmentProp; //!< Properties of the Depth Attachment
		std::vector<uint32_t> mColorAttachments; //!< Renderer IDs of the Color Attachments
		uint32_t mDepthAttachment; //!< Renderer ID of the Depth Attachment

	}; // class OpenGLFrameBuffer
} // namespace Borealis

#endif