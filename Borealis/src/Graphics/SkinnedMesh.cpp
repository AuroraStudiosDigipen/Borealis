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
	SkinnedMesh::SkinnedMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData> &boneData)
	{
		mIndices = indices;
		mIndicesCount = (uint32_t)indices.size();
		mVerticesCount = (uint32_t)vertices.size();
		//mNormals = normals;

		for (int i{}; i < vertices.size(); i++)
		{
			SkinnedVertex vertex;
			vertex.Position = vertices[i];
			vertex.Normal = normals[i];
			vertex.TexCoords = texCoords[i];
			vertex.BoneData = boneData[i];

			mVertices.push_back(vertex);
		}

		SetupMesh();
	}

	SkinnedMesh::~SkinnedMesh()
	{
		
	}

	void SkinnedMesh::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID)
	{
		shader->Bind();

		shader->Set("u_ModelTransform", transform);
		shader->Set("u_EntityID", entityID);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int)mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void SkinnedMesh::SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(SkinnedVertex), &mVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
			&mIndices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, TexCoords));
		//// Tangents
		//glEnableVertexAttribArray(3);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Tangent));
		//// Bitangents
		//glEnableVertexAttribArray(4);
		//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Bitangent));

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mBoneIds));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mWeights));

		glBindVertexArray(0); // Unbind VAO
	}

	std::vector<unsigned int> const& SkinnedMesh::GetIndices() const
	{
		return mIndices;
	}

	std::vector<unsigned int>& SkinnedMesh::GetIndices()
	{
		return mIndices;
	}

	std::vector<SkinnedVertex> const& SkinnedMesh::GetVertices() const
	{
		return mVertices;
	}

	std::vector<SkinnedVertex>& SkinnedMesh::GetVertices()
	{
		return mVertices;
	}

	uint32_t SkinnedMesh::GetVerticesCount() const
	{
		return mVerticesCount;
	}

	void SkinnedMesh::SetVerticesCount(uint32_t count)
	{
		mVerticesCount = count;
	}

	uint32_t SkinnedMesh::GetIndicesCount() const
	{
		return mIndicesCount;
	}

	void SkinnedMesh::SetIndicesCount(uint32_t count)
	{
		mIndicesCount = count;
	}
}