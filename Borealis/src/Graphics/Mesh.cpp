/******************************************************************************/
/*!
\file		Mesh.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 22, 2024
\brief		Declares the class for Mesh for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Graphics/Mesh.hpp>
#include <Core/LoggerSystem.hpp>

#include <Graphics/OpenGL/TextureOpenGLImpl.hpp>

namespace Borealis
{
	void BoundingSphere::Transform(glm::mat4 const& transform)
	{
		Center = glm::vec3(transform * glm::vec4(Center, 1.0f));
		float scaleX = glm::length(glm::vec3(transform[0]));
		float scaleY = glm::length(glm::vec3(transform[1]));
		float scaleZ = glm::length(glm::vec3(transform[2]));
		float maxScale = std::max({ scaleX, scaleY, scaleZ });

		Radius *= maxScale;
	}

	void AABB::Transform(glm::mat4 const& transform)
	{
		// Define the 8 corners of the original AABB
		glm::vec3 corners[8] = {
			minExtent,
			glm::vec3(minExtent.x, minExtent.y, maxExtent.z),
			glm::vec3(minExtent.x, maxExtent.y, minExtent.z),
			glm::vec3(minExtent.x, maxExtent.y, maxExtent.z),
			glm::vec3(maxExtent.x, minExtent.y, minExtent.z),
			glm::vec3(maxExtent.x, minExtent.y, maxExtent.z),
			glm::vec3(maxExtent.x, maxExtent.y, minExtent.z),
			maxExtent
		};

		// Transform each corner
		glm::vec3 transformedMin = glm::vec3(transform * glm::vec4(corners[0], 1.0f));
		glm::vec3 transformedMax = transformedMin;

		for (int i = 1; i < 8; ++i)
		{
			glm::vec3 transformedCorner = glm::vec3(transform * glm::vec4(corners[i], 1.0f));

			// Update min and max extents based on the transformed corner
			transformedMin = glm::min(transformedMin, transformedCorner);
			transformedMax = glm::max(transformedMax, transformedCorner);
		}

		// Update the AABB extents with the transformed min and max
		minExtent = transformedMin;
		maxExtent = transformedMax;
	}

	Mesh::Mesh()// : mVertices(nullptr), mIndices(nullptr), mNormals(nullptr), mTexCoords(nullptr), mVerticesCount(0), mIndicesCount(0), mNormalsCount(0), mTexCoordsCount(0)
	{
	}
	Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords)
	{

		mIndices = indices;
		mIndicesCount = (uint32_t)indices.size();
		mVerticesCount = (uint32_t)vertices.size();

		for (int i{}; i < vertices.size(); i++)
		{
			Vertex vertex;
			vertex.Position = vertices[i];
			vertex.Normal = normals[i];
			vertex.TexCoords = texCoords[i];

			mVertices.push_back(vertex);
		}

		SetupMesh();
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	{
		mVertices = vertices;
		mIndices = indices;

		mVerticesCount = (uint32_t)vertices.size();
		mIndicesCount = (uint32_t)indices.size();

		SetupMesh();
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::Load(const std::string& path)
	{
		BOREALIS_CORE_TRACE("Mesh path is {0}", path);
	}

	void Mesh::SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
			&mIndices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		//// Tangents
		//glEnableVertexAttribArray(3);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(VertexData, Tangent));
		//// Bitangents
		//glEnableVertexAttribArray(4);
		//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(VertexData, Bitangent));

		// Unbind VAO
		glBindVertexArray(0);
	}

	void Mesh::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID)
	{
		shader->Bind();

		shader->Set("u_ModelTransform", transform);
		if(entityID != -1)
		{
			shader->Set("u_EntityID", entityID);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (int)mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	float DistanceSquared(const glm::vec3& a, const glm::vec3& b)
	{
		glm::vec3 diff = a - b;
		return glm::dot(diff, diff);
	}

	void Mesh::GenerateRitterBoundingSphere()
	{
		if (mVertices.empty()) return;

		// Step 1: Find an initial point (p) and a farthest point (q)
		glm::vec3 p = mVertices[0].Position;
		glm::vec3 q = p;
		float maxDistSq = 0.0f;

		for (const auto& vertex : mVertices)
		{
			float distSq = DistanceSquared(p, vertex.Position);
			if (distSq > maxDistSq)
			{
				maxDistSq = distSq;
				q = vertex.Position;
			}
		}

		// Step 2: Use p and q to define initial sphere
		glm::vec3 center = (p + q) / 2.0f;
		float radius = std::sqrt(maxDistSq) / 2.0f;

		// Step 3: Expand sphere to include any outside points
		for (const auto& vertex : mVertices)
		{
			glm::vec3 dir = vertex.Position - center;
			float distSq = glm::dot(dir, dir);

			if (distSq > radius * radius)
			{
				float dist = std::sqrt(distSq);
				float newRadius = (radius + dist) / 2.0f;
				center += (dir / dist) * (newRadius - radius);
				radius = newRadius;
			}
		}

		mBoundingSphere.Center = center;
		mBoundingSphere.Radius = radius;
	}

	void Mesh::GenerateAABB()
	{
		AABB aabb{};
		aabb.minExtent = mVertices[0].Position;
		aabb.maxExtent = mVertices[0].Position;

		for (Vertex const& vertex : mVertices)
		{
			aabb.minExtent.x = std::min(aabb.minExtent.x, vertex.Position.x);
			aabb.minExtent.y = std::min(aabb.minExtent.y, vertex.Position.y);
			aabb.minExtent.z = std::min(aabb.minExtent.z, vertex.Position.z);

			aabb.maxExtent.x = std::max(aabb.maxExtent.x, vertex.Position.x);
			aabb.maxExtent.y = std::max(aabb.maxExtent.y, vertex.Position.y);
			aabb.maxExtent.z = std::max(aabb.maxExtent.z, vertex.Position.z);
		}

		mAABB = aabb;
	}

	BoundingSphere Mesh::GetBoundingSphere()
	{
		return mBoundingSphere;
	}

	AABB Mesh::GetAABB()
	{
		return mAABB;
	}

	void Mesh::DrawQuad()
	{
		if (QuadVAO == 0)
		{
			// Define the vertex positions and texture coordinates for a fullscreen quad
			float quadVertices[] = {
				// Positions    // TexCoords
				-1.0f,  1.0f,   0.0f, 1.0f,
				-1.0f, -1.0f,   0.0f, 0.0f,
				 1.0f, -1.0f,   1.0f, 0.0f,

				-1.0f,  1.0f,   0.0f, 1.0f,
				 1.0f, -1.0f,   1.0f, 0.0f,
				 1.0f,  1.0f,   1.0f, 1.0f
			};

			// Generate and bind a VAO for the quad
			glGenVertexArrays(1, &QuadVAO);
			glGenBuffers(1, &QuadVBO);

			glBindVertexArray(QuadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

			// Position attribute
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			// Texture coordinates attribute
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(QuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}

	void Mesh::DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe, Ref<Shader> shader)
	{
		if (CubeVAO == 0)
		{
			const GLfloat cubeVertices[] = 
			{
				// Positions        
				-0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f,

				-0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f
			};

			const GLuint cubeIndices[] = 
			{
				0, 1, 2, 2, 3, 0, // Front face
				4, 5, 6, 6, 7, 4, // Back face
				4, 5, 1, 1, 0, 4, // Bottom face
				3, 2, 6, 6, 7, 3, // Top face
				4, 0, 3, 3, 7, 4, // Left face
				5, 1, 2, 2, 6, 5  // Right face
			};

			glGenVertexArrays(1, &CubeVAO);
			glGenBuffers(1, &CubeVBO);
			glGenBuffers(1, &CubeEBO);

			glBindVertexArray(CubeVAO);
			glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // Vertex positions
			glEnableVertexAttribArray(0);
		}

		glBindVertexArray(CubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);

		shader->Bind();

		// Pass the model matrix to the shader
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, translation);
		//model = glm::translate(model, translation);
		model = glm::scale(model, maxExtent - minExtent);

		shader->Set("u_ModelTransform", model);

		shader->Set("u_Color", color);

		// Toggle wireframe if requested
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Draw the cube
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Reset wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Mesh::DrawSphere(glm::vec3 center, float radius, glm::vec4 color, bool wireframe, Ref<Shader> shader, SphereSides side)
	{
		static GLuint SphereVAO = 0, SphereVBO = 0, SphereEBO = 0;
		static int sphereIndexCount = 0;
		static std::vector<GLuint> fullIndices;
		const int latitudeSegments = 16;
		const int longitudeSegments = 16;

		if (SphereVAO == 0)
		{
			std::vector<glm::vec3> vertices;
			std::vector<GLuint> indices;

			// Generate vertices
			for (int y = 0; y <= latitudeSegments; ++y)
			{
				float theta = y * glm::pi<float>() / latitudeSegments; // Latitude angle
				float sinTheta = sin(theta) * 0.5f;
				float cosTheta = cos(theta) * 0.5f;

				for (int x = 0; x <= longitudeSegments; ++x)
				{
					float phi = x * 2.0f * glm::pi<float>() / longitudeSegments; // Longitude angle
					float sinPhi = sin(phi);
					float cosPhi = cos(phi);

					glm::vec3 position(
						cosPhi * sinTheta,
						cosTheta,
						sinPhi * sinTheta
					);

					vertices.push_back(position);
				}
			}

			// Generate indices
			for (int y = 0; y < latitudeSegments; ++y)
			{
				for (int x = 0; x < longitudeSegments; ++x)
				{
					int first = y * (longitudeSegments + 1) + x;
					int second = first + longitudeSegments + 1;

					indices.push_back(first);
					indices.push_back(second);
					indices.push_back(first + 1);

					indices.push_back(second);
					indices.push_back(second + 1);
					indices.push_back(first + 1);
				}
			}

			fullIndices = indices;
			sphereIndexCount = static_cast<int>(indices.size());

			// Create buffers
			glGenVertexArrays(1, &SphereVAO);
			glGenBuffers(1, &SphereVBO);
			glGenBuffers(1, &SphereEBO);

			glBindVertexArray(SphereVAO);

			glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
		}

		// Bind the VAO
		glBindVertexArray(SphereVAO);

		shader->Bind();

		// Create and set model matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, center);
		model = glm::scale(model, glm::vec3(radius));

		shader->Set("u_ModelTransform", model);
		shader->Set("u_Color", color);

		std::vector<GLuint> partialIndices;
		if (side != SphereSides::BOTH)
		{
			int cutoff = latitudeSegments / 2;
			for (int y = 0; y < latitudeSegments; ++y)
			{
				if ((side == SphereSides::TOP && y >= cutoff) || (side == SphereSides::BOTTOM && y < cutoff))
					continue; // Skip the other half

				for (int x = 0; x < longitudeSegments; ++x)
				{
					int first = y * (longitudeSegments + 1) + x;
					int second = first + longitudeSegments + 1;

					partialIndices.push_back(first);
					partialIndices.push_back(second);
					partialIndices.push_back(first + 1);

					partialIndices.push_back(second);
					partialIndices.push_back(second + 1);
					partialIndices.push_back(first + 1);
				}
			}

			// Update the EBO with partial indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, partialIndices.size() * sizeof(GLuint), partialIndices.data(), GL_DYNAMIC_DRAW);
			sphereIndexCount = static_cast<int>(partialIndices.size());
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, fullIndices.size() * sizeof(GLuint), fullIndices.data(), GL_DYNAMIC_DRAW);
			sphereIndexCount = static_cast<int>(fullIndices.size());
		}

		// Toggle wireframe mode
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Draw the sphere
		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

		// Reset polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
	}

	void Mesh::DrawCylinder(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe, Ref<Shader> shader)
	{
		static GLuint CylinderVAO = 0, CylinderVBO = 0, CylinderEBO = 0;
		const int segments = 16; // Adjust for detail level

		if (CylinderVAO == 0)
		{
			std::vector<GLfloat> vertices;
			std::vector<GLuint> indices;

			// Generate vertices
			for (int i = 0; i <= segments; ++i)
			{
				float angle = glm::radians(360.0f * i / segments);
				float x = cos(angle) * 0.5f;
				float z = sin(angle) * 0.5f;

				// Bottom circle
				vertices.push_back(x);
				vertices.push_back(-1.f / 2.0f);
				vertices.push_back(z);

				// Top circle
				vertices.push_back(x);
				vertices.push_back(1.f / 2.0f);
				vertices.push_back(z);
			}

			// Indices for the curved surface
			for (int i = 0; i < segments; ++i)
			{
				int bottom1 = i * 2;
				int top1 = bottom1 + 1;
				int bottom2 = (i + 1) * 2;
				int top2 = bottom2 + 1;

				// First triangle of the quad
				indices.push_back(bottom1);
				indices.push_back(top1);
				indices.push_back(bottom2);

				// Second triangle of the quad
				indices.push_back(bottom2);
				indices.push_back(top1);
				indices.push_back(top2);
			}

			//// Indices for the bottom cap
			//int bottomCenterIndex = vertices.size() / 3;
			//vertices.push_back(0.0f);                // x
			//vertices.push_back(-height / 2.0f);     // y
			//vertices.push_back(0.0f);                // z

			//for (int i = 0; i < segments; ++i)
			//{
			//	int next = (i + 1) % segments;
			//	indices.push_back(bottomCenterIndex);
			//	indices.push_back(i * 2);
			//	indices.push_back(next * 2);
			//}

			//// Indices for the top cap
			//int topCenterIndex = vertices.size() / 3;
			//vertices.push_back(0.0f);               // x
			//vertices.push_back(height / 2.0f);      // y
			//vertices.push_back(0.0f);               // z

			//for (int i = 0; i < segments; ++i)
			//{
			//	int next = (i + 1) % segments;
			//	indices.push_back(topCenterIndex);
			//	indices.push_back(i * 2 + 1);
			//	indices.push_back(next * 2 + 1);
			//}

			// Create VAO, VBO, EBO
			glGenVertexArrays(1, &CylinderVAO);
			glGenBuffers(1, &CylinderVBO);
			glGenBuffers(1, &CylinderEBO);

			glBindVertexArray(CylinderVAO);

			glBindBuffer(GL_ARRAY_BUFFER, CylinderVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CylinderEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
		}

		// Bind and configure shader
		shader->Bind();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, center);
		model = glm::scale(model, glm::vec3(radius, height, radius));

		shader->Set("u_ModelTransform", model);
		shader->Set("u_Color", color);

		// Set wireframe mode if requested
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Draw the cylinder
		glBindVertexArray(CylinderVAO);
		glDrawElements(GL_TRIANGLES, (segments * 6) + (segments * 3 * 2), GL_UNSIGNED_INT, 0);

		// Reset to fill mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
	}

	void Mesh::DrawCapsule(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe, Ref<Shader> shader)
	{
		// Draw cylinder
		DrawCylinder(center, radius, height, color, wireframe, shader);

		glm::vec3 topSphereCenter = center;
		topSphereCenter.y += height*0.5f;
		DrawSphere(topSphereCenter, radius, color, wireframe, shader, SphereSides::TOP);

		glm::vec3 bottomSphereCenter = center;
		bottomSphereCenter.y -= height*0.5f;
		DrawSphere(bottomSphereCenter, radius, color, wireframe, shader, SphereSides::BOTTOM);
	}

	std::vector<unsigned int> const& Mesh::GetIndices() const
	{
		return mIndices;
	}

	std::vector<unsigned int>& Mesh::GetIndices()
	{
		return mIndices;
	}

	std::vector<Vertex> const& Mesh::GetVertices() const
	{
		return mVertices;
	}

	std::vector<Vertex>& Mesh::GetVertices()
	{
		return mVertices;
	}

	uint32_t Mesh::GetVerticesCount() const
	{
		return mVerticesCount;
	}

	void Mesh::SetVerticesCount(uint32_t count)
	{
		mVerticesCount = count;
	}

	uint32_t Mesh::GetIndicesCount() const
	{
		return mIndicesCount;
	}

	void Mesh::SetIndicesCount(uint32_t count)
	{
		mIndicesCount = count;
	}

	void Mesh::ComputeTangents() 
	{
		mVerticesData.resize(mVertices.size());
		// Initialize tangents and bitangents to zero
		for (size_t i = 0; i < mVerticesData.size(); i++) {
			mVerticesData[i].Tangent = glm::vec3(0.0f);
			mVerticesData[i].Bitangent = glm::vec3(0.0f);
		}

		// Loop over each triangle
		for (size_t i = 0; i < mIndices.size(); i += 3) {
			Vertex& v0 = mVertices[mIndices[i]];
			Vertex& v1 = mVertices[mIndices[i + 1]];
			Vertex& v2 = mVertices[mIndices[i + 2]];

			VertexData& vd0 = mVerticesData[mIndices[i]];
			VertexData& vd1 = mVerticesData[mIndices[i + 1]];
			VertexData& vd2 = mVerticesData[mIndices[i + 2]];

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
			vd0.Tangent += tangent;
			vd1.Tangent += tangent;
			vd2.Tangent += tangent;

			vd0.Bitangent += bitangent;
			vd1.Bitangent += bitangent;
			vd2.Bitangent += bitangent;
		}

		// Normalize the tangents and bitangents
		for (size_t i = 0; i < mVertices.size(); i++) {
			mVerticesData[i].Tangent = glm::normalize(mVerticesData[i].Tangent);
			mVerticesData[i].Bitangent = glm::normalize(mVerticesData[i].Bitangent);
		}
	}

	//void Mesh::SetVertices(const std::vector<glm::vec3>& vertices)
	//{
	//	const size_t& size = vertices.size();
	//	if (size > 0)
	//	{
	//		if (mVerticesCount == 0)
	//		{
	//			mVertices = new glm::vec3[size];
	//			memcpy(mVertices, vertices.data(), size * sizeof(glm::vec3));
	//		}
	//		else
	//		{
	//			delete[] mVertices;
	//			mVertices = new glm::vec3[size];
	//			memcpy(mVertices, vertices.data(), size * sizeof(glm::vec3));
	//		}
	//	}
	//	else
	//	{
	//		BOREALIS_CORE_WARN("Mesh::SetVertices() - No vertices to set");
	//		return;
	//	}
	//	mVerticesCount = (uint32_t)size;
	//}
	//void Mesh::SetIndices(const std::vector<unsigned int>& indices)
	//{
	//	const size_t& size = indices.size();
	//	if (size > 0)
	//	{
	//		if (mIndicesCount == 0)
	//		{
	//			mIndices = new unsigned[size];
	//			memcpy(mIndices, indices.data(), size * sizeof(unsigned));
	//		}
	//		else
	//		{
	//			delete[] mIndices;
	//			mIndices = new unsigned[size];
	//			memcpy(mIndices, indices.data(), size * sizeof(unsigned));
	//		}
	//	}
	//	else
	//	{
	//		BOREALIS_CORE_WARN("Mesh::SetIndices() - No index to set");
	//		return;
	//	}
	//	mIndicesCount = (uint32_t)size;
	//}
	//void Mesh::SetNormals(const std::vector<glm::vec3>& normals)
	//{
	//	const size_t& size = normals.size();
	//	if (size > 0)
	//	{
	//		if (mNormalsCount == 0)
	//		{
	//			mNormals = new glm::vec3[size];
	//			memcpy(mNormals, normals.data(), size * sizeof(glm::vec3));
	//		}
	//		else
	//		{
	//			delete[] mNormals;
	//			mNormals = new glm::vec3[size];
	//			memcpy(mNormals, normals.data(), size * sizeof(glm::vec3));
	//		}
	//	}
	//	else
	//	{
	//		BOREALIS_CORE_WARN("Mesh::SetNormals() - No normals to set");
	//		return;
	//	}
	//	mNormalsCount = (uint32_t)size;
	//}
	//void Mesh::SetTexCoords(const std::vector<glm::vec2>& texCoords)
	//{
	//	const size_t& size = texCoords.size();
	//	if (size > 0)
	//	{
	//		if (mTexCoordsCount == 0)
	//		{
	//			mTexCoords = new glm::vec2[size];
	//			memcpy(mTexCoords, texCoords.data(), size * sizeof(glm::vec2));
	//		}
	//		else
	//		{
	//			delete[] mTexCoords;
	//			mTexCoords = new glm::vec2[size];
	//			memcpy(mTexCoords, texCoords.data(), size * sizeof(glm::vec2));
	//		}
	//	}
	//	else
	//	{
	//		BOREALIS_CORE_WARN("Mesh::SetTexCoords() - No TexCoords to set");
	//		return;
	//	}
	//	mTexCoordsCount = (uint32_t)size;
	//}
	//const glm::vec3* Mesh::GetVertices() const
	//{
	//	return mVertices;
	//}
	//const unsigned int* Mesh::GetIndices() const
	//{
	//	return mIndices;
	//}
	//const glm::vec3* Mesh::GetNormals() const
	//{
	//	return mNormals;
	//}
	//const glm::vec2* Mesh::GetTexCoords() const
	//{
	//	return mTexCoords;
	//}
	//uint32_t Mesh::GetVerticesCount() const
	//{
	//	return mVerticesCount;
	//}
	//uint32_t Mesh::GetIndicesCount() const
	//{
	//	return mIndicesCount;
	//}
	//uint32_t Mesh::GetNormalsCount() const
	//{
	//	return mNormalsCount;
	//}
	//uint32_t Mesh::GetTexCoordsCount() const
	//{
	//	return mTexCoordsCount;
	//}
}