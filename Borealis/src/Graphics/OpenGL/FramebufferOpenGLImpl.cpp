/******************************************************************************/
/*!
\file		FramebufferOpenGLImpl.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 11, 2024
\brief		Defines the OpenGL class for Frame buffers

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include <BorealisPCH.hpp>
#include <glad/glad.h>
#include <Graphics/OpenGL/FramebufferOpenGLImpl.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/Utils.hpp>
namespace Borealis
{

	static const uint32_t s_MaxFramebufferSize = 16384;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProperties& props) : mProps(props), mRendererID(0)
	{
		for (auto spec : mProps.Attachments.mAttachments)
		{
			if (!GraphicsUtils::IsDepthFormat(spec.mTextureFormat))
			{
				mColorAttachmentProps.emplace_back(spec);
			}
			else
			{
				mDepthAttachmentProp = spec;
			}
		}
		Recreate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &mRendererID);
		glDeleteTextures(1, &mDepthAttachment);
		glDeleteTextures((GLsizei)mColorAttachments.size(), mColorAttachments.data());
	}
	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glViewport(0, 0, mProps.Width, mProps.Height);
	}
	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			BOREALIS_CORE_WARN("Framebuffer size is invalid, Width: {}, Height {}", width, height);
			return;
		}
		mProps.Width = width;
		mProps.Height = height;
		Recreate();
	}
	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glClearBufferiv(GL_COLOR, attachmentIndex, &value);
	}
	void OpenGLFrameBuffer::Recreate()
	{
		if (mRendererID)
		{
			glDeleteFramebuffers(1, &mRendererID);
			glDeleteTextures(1, &mDepthAttachment);
			glDeleteTextures((GLsizei)mColorAttachments.size(), mColorAttachments.data());
			mColorAttachments.clear();
			mDepthAttachment = 0;
		}

		glGenFramebuffers(1, &mRendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		
		bool multisample = mProps.Samples > 1;

		if (!mColorAttachmentProps.empty())
		{
			mColorAttachments.resize(mColorAttachmentProps.size());
			GraphicsUtils::CreateTextures(multisample, mColorAttachments.data(), mColorAttachments.size());


			for (int i = 0; i < (int)mColorAttachments.size(); i++)
			{
				GraphicsUtils::BindTexture(multisample, mColorAttachments[i]);
				switch (mColorAttachmentProps[i].mTextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					GraphicsUtils::AttachColorTexture(mColorAttachments[i], mProps.Samples, GL_RGBA8, GL_RGBA, mProps.Width, mProps.Height, i);
					break;

				case FramebufferTextureFormat::RedInteger:
					GraphicsUtils::AttachColorTexture(mColorAttachments[i], mProps.Samples, GL_R32I, GL_RED_INTEGER, mProps.Width, mProps.Height, i);
					break;
				}
			}
		}

		if (mDepthAttachmentProp.mTextureFormat != FramebufferTextureFormat::None)
		{
			GraphicsUtils::CreateTextures(multisample, &mDepthAttachment, 1);
			GraphicsUtils::BindTexture(multisample, mDepthAttachment);
			switch (mDepthAttachmentProp.mTextureFormat)
			{
			case FramebufferTextureFormat::Depth24Stencil8:
				GraphicsUtils::AttachDepthTexture(mDepthAttachment, mProps.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mProps.Width, mProps.Height);
				break;
			}
		}

		if (mColorAttachments.size() > 1)
		{
			BOREALIS_CORE_ASSERT(mColorAttachments.size() <= 4,"Too many color attachments!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)mColorAttachments.size(), buffers);

		}
		else if (mColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		BOREALIS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
}
