/******************************************************************************/
/*!
\file		BufferOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 6, 2024
\brief		Declares the OpenGL class for Graphics Buffer

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BUFFER_OPENGL_IMPL_HPP
#define BUFFER_OPENGL_IMPL_HPP
#include <Graphics/Buffer.hpp>
namespace Borealis
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor for OpenGLVertexBuffer
			\param[in] vertices
				Pointer to the vertices
			\param[in] size
				Size of the vertices
		*************************************************************************/
		OpenGLVertexBuffer(float* vertices, uint32_t size);

		/*!***********************************************************************
			\brief
				Constructor for OpenGLVertexBuffer
			\param[in] size
				Size of the vertices
		*************************************************************************/
		OpenGLVertexBuffer(uint32_t size);

		/*!***********************************************************************
			\brief
				Destructor for OpenGLVertexBuffer
		*************************************************************************/
		~OpenGLVertexBuffer();

		/*!***********************************************************************
			\brief
				Set the layout of the buffer
			\param[in] layout
				Layout of the buffer
		*************************************************************************/
		void SetLayout(const BufferLayout& layout) override { mLayout = layout; }

		/*!***********************************************************************
			\brief
				Set the data of the buffer
			\param[in] data
				Pointer to the data
			\param[in] size
				Size of the data
		*************************************************************************/
		void SetData(const void* data, uint32_t size) override;

		/*!***********************************************************************
			\brief
				Get the layout of the buffer
			\return
				Layout of the buffer
		*************************************************************************/
		const BufferLayout& GetLayout() const override { return mLayout; }

		/*!***********************************************************************
			\brief
				Get the renderer ID of the buffer
			\return
				Renderer ID of the buffer
		*************************************************************************/
		void Bind() const override;

		/*!***********************************************************************
			\brief
				Unbind the buffer
		*************************************************************************/
		void Unbind() const override;
	private:
		uint32_t mRendererID; //!< Renderer ID of the buffer
		BufferLayout mLayout; //!< Layout of the buffer
	};

	class OpenGLElementBuffer : public ElementBuffer
	{
	public:
		/*!***********************************************************************
			\brief 
				Default constructor of OpenGLElementBuffer
			\param[in] indices
				Pointer to the indices
			\param[in] count
				Count of the indices
		*************************************************************************/
		OpenGLElementBuffer(uint32_t* indices, uint32_t count);

		/*!***********************************************************************
			\brief
				Destructor of OpenGLElementBuffer
		*************************************************************************/
		~OpenGLElementBuffer();

		/*!***********************************************************************
			\brief
				Get the count of the buffer
			\return
				Count of the buffer
		*************************************************************************/
		uint32_t GetCount() const override { return mCount; }

		/*!***********************************************************************
			\brief
				Bind the buffer
		*************************************************************************/
		void Bind() const override;

		/*!***********************************************************************
			\brief
				Unbind the buffer
		*************************************************************************/
		void Unbind() const override;
	private:
		uint32_t mRendererID = 0; //!< Renderer ID of the buffer
		uint32_t mCount;	  //!< Count of the buffer
	}; // class OpenGLElementBuffer

} // namespace Borealis


#endif