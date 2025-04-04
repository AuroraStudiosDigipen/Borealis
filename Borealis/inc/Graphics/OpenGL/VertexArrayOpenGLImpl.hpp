/******************************************************************************/
/*!
\file		VertexArrayOpenGLImpl.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the OpenGL class for Vertex Array

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#ifndef VERTEX_ARRAY_OPENGL_IMPL_HPP
#define VERTEX_ARRAY_OPENGL_IMPL_HPP
#include <vector>
#include <Graphics/VertexArray.hpp>

namespace Borealis
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for OpenGLVertexArray
		*************************************************************************/
		OpenGLVertexArray();

		/*!***********************************************************************
			\brief
				Destructor for OpenGLVertexArray
		*************************************************************************/
		~OpenGLVertexArray() override;

		/*!***********************************************************************
			\brief
				Bind the Vertex Array
		*************************************************************************/
		void Bind() const override;

		/*!***********************************************************************
			\brief
				Unbind the Vertex Array
		*************************************************************************/
		void Unbind() const override;

		/*!***********************************************************************
			\brief
				Add a Vertex Buffer to the Vertex Array
			\param VBO
				The Vertex Buffer to add
		*************************************************************************/
		void AddVertexBuffer(const Ref<VertexBuffer>& VBO) override;

		/*!***********************************************************************
			\brief
				Set the Element Buffer of the Vertex Array
			\param EBO
				The Element Buffer to set
		*************************************************************************/
		void SetElementBuffer(const Ref<ElementBuffer>& EBO) override;


		/*!***********************************************************************
			\brief
				Get the Vertex Buffers of the Vertex Array
			\return
				The Vertex Buffers of the Vertex Array
		*************************************************************************/
		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return mVBOs; }

		/*!***********************************************************************
			\brief
				Get the Element Buffer of the Vertex Array
			\return
				The Element Buffer of the Vertex Array
		*************************************************************************/
		const Ref<ElementBuffer> GetElementBuffer() const override { return mEBO; }
	private:
		std::vector<Ref<VertexBuffer>> mVBOs; //!< The Vertex Buffers of the Vertex Array
		Ref<ElementBuffer> mEBO; //!< The Element Buffer of the Vertex Array
		uint32_t mRendererID = 0; //!< The Renderer ID of the Vertex Array
	};
}

#endif

