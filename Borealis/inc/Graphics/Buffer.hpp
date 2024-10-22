/******************************************************************************/
/*!
\file		Buffer.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 6, 2024
\brief		Declares the Generic class for Graphics Buffer

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <vector>
#include <string>
#include <Core/LoggerSystem.hpp>
#include <Core/Core.hpp>
namespace Borealis
{
	// Virtual classes for if we ever expand to non OpenGL APIs such as DX

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		BOREALIS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string mName; // Name of the element
		size_t mOffset; // Offset of the element
		uint32_t mSize;  // Size of the element
		ShaderDataType mType; // Type of the element
		bool mNormalized; // Is the element normalized

		/*!***********************************************************************
			\brief
				Constructor of the BufferElement
			\param[in] type
				The type of the element
			\param[in] name
				The name of the element
			param[in] normalized
				Is the element normalized
		*************************************************************************/
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: mName(name), mType(type), mSize(ShaderDataTypeSize(type)), mOffset(0), mNormalized(normalized) {}

		/*!***********************************************************************
			\brief
				Get the component count of the type
			\return
				The component count of the type
		*************************************************************************/
		uint32_t GetComponentCount() const;
	}; // Struct BufferElement


	class BufferLayout
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor of the BufferLayout
			\param[in] elements
				The elements of the buffer layout
		*************************************************************************/
		BufferLayout(std::initializer_list<BufferElement> elements)
			: mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		/*!***********************************************************************
			\brief
				Default Constructor of the BufferLayout
		*************************************************************************/
		BufferLayout() {};

		// Iterators for the buffer layout
		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }

		/*!***********************************************************************
			\brief
				Get the elements of the buffer layout
			\return
				The elements of the buffer layout
		*************************************************************************/
		inline const std::vector<BufferElement>& GetElements() const { return mElements; }

		/*!***********************************************************************
			\brief
				Get the stride of the buffer layout
			\return
				The stride of the buffer layout
		*************************************************************************/
		inline uint32_t GetStride() const { return mStride; }


	private:
		/*!***********************************************************************
			\brief
				Calculate the offsets and stride of the buffer layout
		*************************************************************************/
		void CalculateOffsetsAndStride();
		std::vector<BufferElement> mElements; // Elements of the buffer layout
		uint32_t mStride; // Stride of the buffer layout
	}; // Class BufferLayout

	class VertexBuffer
	{
	public :
		/*!***********************************************************************
			\brief
				Destructor of the VertexBuffer Virtual Class
		*************************************************************************/
		virtual ~VertexBuffer() {}

		/*!***********************************************************************
			\brief
				Bind the VertexBuffer
		*************************************************************************/
		virtual void Bind() const = 0;

		/*!***********************************************************************
			\brief
				Unbind the VertexBuffer
		*************************************************************************/
		virtual void Unbind() const = 0;

		/*!***********************************************************************
			\brief
				Set the layout of the VertexBuffer
			\param[in] layout
				The layout of the VertexBuffer
		*************************************************************************/
		virtual void SetLayout(const BufferLayout& layout) = 0;

		/*!***********************************************************************
			\brief
				Set the data of the VertexBuffer
			\param[in] data
				The data of the VertexBuffer
			\param[in] size
				The size of the data
		*************************************************************************/
		virtual void SetData(const void* data, uint32_t size) = 0;

		/*!***********************************************************************
			\brief
				Get the layout of the VertexBuffer
			\return
				The layout of the VertexBuffer
		*************************************************************************/
		virtual const BufferLayout& GetLayout() const = 0;

		/*!***********************************************************************
			\brief
				Create a VertexBuffer with a size depending on Graphics API
			\param[in] size
				The size of the VertexBuffer
			\return
				The VertexBuffer
		*************************************************************************/
		static Ref<VertexBuffer> Create(uint32_t size);

		/*!***********************************************************************
			\brief
				Create a VertexBuffer with vertices depending on Graphics API
			\param[in] vertices
				The vertices of the VertexBuffer
			\param[in] size
				The size of the VertexBuffer
			\return
				The VertexBuffer
		*************************************************************************/
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	}; // Class VertexBuffer

	class ElementBuffer
	{
	public: 
		/*!***********************************************************************
			\brief
				Destructor of the ElementBuffer Virtual Class
		*************************************************************************/
		virtual ~ElementBuffer() {}

		/*!***********************************************************************
			\brief
				Bind the ElementBuffer
		*************************************************************************/
		virtual void Bind() const = 0;

		/*!***********************************************************************
			\brief
				Unbind the ElementBuffer
		*************************************************************************/
		virtual void Unbind() const = 0;

		/*!***********************************************************************
			\brief
				Get the count of the ElementBuffer
			\return
				The count of the ElementBuffer
		*************************************************************************/
		virtual uint32_t GetCount() const = 0;

		/*!***********************************************************************
			\brief
				Create an ElementBuffer with indices depending on Graphics API
			\param[in] indices
				The indices of the ElementBuffer
			\param[in] count
				The count of the ElementBuffer
			\return
				The ElementBuffer
		*************************************************************************/
		static Ref<ElementBuffer> Create(uint32_t* indices, uint32_t count);
	};
}

#endif