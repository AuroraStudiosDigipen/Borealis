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
		PROFILE_FUNCTION();
		shader->Bind();

		shader->Set("u_ModelTransform", transform);
		shader->Set("u_EntityID", entityID);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int)mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void SkinnedMesh::SetupMesh()
	{
		ComputeTangents();
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
		// Tangents
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Tangent));
		// Bitangents
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Bitangent));

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mBoneIds));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneData.mWeights));

		glBindVertexArray(0); // Unbind VAO
	}

	void SkinnedMesh::ComputeTangents()
	{
		//mVertices.resize(mVertices.size());
		// Initialize tangents and bitangents to zero
		for (size_t i = 0; i < mVertices.size(); i++) {
			mVertices[i].Tangent = glm::vec3(0.0f);
			mVertices[i].Bitangent = glm::vec3(0.0f);
		}

		// Loop over each triangle
		for (size_t i = 0; i < mIndices.size(); i += 3) {
			SkinnedVertex& v0 = mVertices[mIndices[i]];
			SkinnedVertex& v1 = mVertices[mIndices[i + 1]];
			SkinnedVertex& v2 = mVertices[mIndices[i + 2]];

			// Positions
			glm::vec3& p0 = v0.Position;
			glm::vec3& p1 = v1.Position;
			glm::vec3& p2 = v2.Position;

			// Texture coordinates
			glm::vec2& uv0 = v0.TexCoords;
			glm::vec2& uv1 = v1.TexCoords;
			glm::vec2& uv2 = v2.TexCoords;

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = p1 - p0;
			glm::vec3 deltaPos2 = p2 - p0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent = f * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
			glm::vec3 bitangent = f * (-deltaPos1 * deltaUV2.x + deltaPos2 * deltaUV1.x);

			// Accumulate the tangents and bitangents
			v0.Tangent += tangent;
			v1.Tangent += tangent;
			v2.Tangent += tangent;

			v0.Bitangent += bitangent;
			v1.Bitangent += bitangent;
			v2.Bitangent += bitangent;
		}

		// Normalize the tangents and bitangents
		for (size_t i = 0; i < mVertices.size(); i++) {
			mVertices[i].Tangent = glm::normalize(mVertices[i].Tangent);
			mVertices[i].Bitangent = glm::normalize(mVertices[i].Bitangent);
		}
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