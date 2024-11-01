/******************************************************************************/
/*!
\file		Mesh.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 22, 2024
\brief		Declares the class for Mesh for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MESH_HPP
#define MESH_HPP
#include <vector>
#include <glm/glm.hpp>

#include <Graphics/Shader.hpp>

namespace Borealis
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;

	};

	struct VertexData
	{
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	class Mesh
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor for a Mesh
		*************************************************************************/
		Mesh();

		/*!***********************************************************************
			\brief
				Constructor for a Mesh
			\param vertices
				Vertices of the mesh
			\param indices
				Indices of the mesh
			\param normals
				Normals of the mesh
			\param texCoords
				Texture coordinates of the mesh
		*************************************************************************/
		Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords);

		//Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData> boneData);

		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

		/*!***********************************************************************
			\brief
				Destructor for a Mesh
		*************************************************************************/
		~Mesh();

		/*!***********************************************************************
			\brief
				Load mesh from path
		*************************************************************************/
		void Load(const std::string& path);

		/*!***********************************************************************
			\brief
				Set up mesh
		*************************************************************************/
		void SetupMesh();

		/*!***********************************************************************
			\brief
				Draw mesh
		*************************************************************************/
		void Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID);

		/*!***********************************************************************
			\brief
				Getters and setters
		*************************************************************************/
		std::vector<unsigned int> const& GetIndices() const;
		std::vector<unsigned int>& GetIndices();

		std::vector<Vertex> const& GetVertices() const;
		std::vector<Vertex>& GetVertices();

		uint32_t GetVerticesCount() const;
		void SetVerticesCount(uint32_t count);

		uint32_t GetIndicesCount() const;
		void SetIndicesCount(uint32_t count);

	private:
		std::vector<unsigned int> mIndices;
		std::vector<Vertex> mVertices;
		std::vector<VertexData> mVerticesData;

		uint32_t mVerticesCount; // Number of vertices
		uint32_t mIndicesCount; // Number of indices

		unsigned int VAO, VBO, EBO;


		void ComputeTangents();

	}; // class Mesh
} // namespace Borealis
#endif