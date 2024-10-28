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

#include "Mesh.hpp"

namespace Borealis
{
	struct SkinnedVertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		VertexBoneData BoneData;
	};

	class SkinnedMesh : public Mesh
	{
	public:
		SkinnedMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData> boneData);

		~SkinnedMesh();

		void Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID) override;

	private:
		void SetupMesh() override;

		std::vector<unsigned int> mIndices;
		std::vector<SkinnedVertex> mVertices;
		std::vector<VertexData> mVerticesData;

		uint32_t mVerticesCount; // Number of vertices
		uint32_t mIndicesCount; // Number of indices

		unsigned int VAO, VBO, EBO;
	};
}

#endif
