/******************************************************************************/
/*!
\file		SkinnedMesh.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class for Skinned Mesh for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef SKINNEDMESH_HPP
#define SKINNEDMESH_HPP

#include <vector>
#include <glm/glm.hpp>

#include <Graphics/Mesh.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Animation/VertexBone.hpp>

namespace Borealis
{
	struct SkinnedVertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		VertexBoneData BoneData;
	};

	struct SkinnedVertexOld {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		VertexBoneData BoneData;
	};

	class SkinnedMesh
	{
	public:
		SkinnedMesh() = default;
		SkinnedMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData> & boneData);

		~SkinnedMesh();

		void Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID);

		/*!***********************************************************************
			\brief
				Getters and setters
		*************************************************************************/
		std::vector<unsigned int> const& GetIndices() const;
		std::vector<unsigned int>& GetIndices();

		std::vector<SkinnedVertex> const& GetVertices() const;
		std::vector<SkinnedVertex>& GetVertices();

		uint32_t GetVerticesCount() const;
		void SetVerticesCount(uint32_t count);

		uint32_t GetIndicesCount() const;
		void SetIndicesCount(uint32_t count);

		void SetupMesh();
	private:

		void ComputeTangents();

		std::vector<unsigned int> mIndices;
		std::vector<SkinnedVertex> mVertices;

		uint32_t mVerticesCount; // Number of vertices
		uint32_t mIndicesCount; // Number of indices

		unsigned int VAO, VBO, EBO;
	};
}

#endif
