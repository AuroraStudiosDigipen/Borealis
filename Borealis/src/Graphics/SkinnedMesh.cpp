/******************************************************************************/
/*!
\file		SkinnedMesh.cpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class for Skinned Mesh for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include "Graphics/SkinnedMesh.hpp"
#include <Graphics/OpenGL/TextureOpenGLImpl.hpp>

namespace Borealis
{
	SkinnedMesh::SkinnedMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData> boneData)
		: Mesh(vertices, indices, normals, texCoords)
	{
		for (int i{}; i < vertices.size(); i++)
		{
			mVertices[i].BoneData = boneData[i];
		}

		SetupMesh();
	}

	SkinnedMesh::~SkinnedMesh()
	{
		
	}

	void SkinnedMesh::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID)
	{
		Mesh::Draw(transform, shader, entityID);
	}

	void SkinnedMesh::SetupMesh()
	{
		Mesh::SetupMesh();

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mBoneIds));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mWeights));

		glBindVertexArray(0); // Unbind VAO
	}
}