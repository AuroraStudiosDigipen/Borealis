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
#include <glm/gtc/matrix_transform.hpp>

#include <Graphics/Shader.hpp>

namespace Borealis
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

	};

	struct BoundingSphere
	{
		glm::vec3 Center;
		float Radius;

		void Transform(glm::mat4 const& transform);
	};

	struct AABB
	{
		glm::vec3 minExtent;
		glm::vec3 maxExtent;

		void Transform(glm::mat4 const& transform);
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

		void GenerateRitterBoundingSphere();

		void GenerateAABB();

		BoundingSphere GetBoundingSphere();

		AABB GetAABB();

		static void DrawQuad();

		static void DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe, Ref<Shader> shader);

		static void DrawCubeMap();

		enum class SphereSides
		{
			BOTH,
			TOP,
			BOTTOM
		};

		static void DrawSphere(glm::vec3 center, float radius, glm::vec4 color, bool wireframe, Ref<Shader> shader, SphereSides side = SphereSides::BOTH);

		static void DrawCylinder(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe, Ref<Shader> shader);

		static void DrawCapsule(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe, Ref<Shader> shader);

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

		uint32_t mVerticesCount; // Number of vertices
		uint32_t mIndicesCount; // Number of indices


		unsigned int VAO, VBO, EBO;

		BoundingSphere mBoundingSphere;
		AABB mAABB;

		void ComputeTangents();

		inline static unsigned int QuadVAO = 0, QuadVBO = 0;
		inline static unsigned int CubeVAO = 0, CubeVBO = 0, CubeEBO = 0;

	}; // class Mesh
} // namespace Borealis
#endif