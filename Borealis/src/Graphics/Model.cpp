/******************************************************************************/
/*!
\file		Model.cpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 12, 2024
\brief		Declares the class for Model for rendering

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Graphics/Model.hpp>

namespace Borealis
{
	void Model::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID)
	{
		for (auto mesh : mMeshes)
		{
			mesh.Draw(transform, shader, entityID);
		}
	}

	void Model::LoadModel(std::filesystem::path const& path)
	{
		std::ifstream inFile(path, std::ios::binary);

		uint32_t meshCount;
		inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

		mMeshes.resize(meshCount);

		for (Mesh& mesh : mMeshes) 
		{
			//always the same?
			uint32_t verticesCount, indicesCount;
			inFile.read(reinterpret_cast<char*>(&verticesCount), sizeof(verticesCount));
			mesh.SetVerticesCount(verticesCount);
			inFile.read(reinterpret_cast<char*>(&indicesCount), sizeof(indicesCount));
			mesh.SetIndicesCount(indicesCount);

			mesh.GetVertices().resize(verticesCount);
			mesh.GetIndices().resize(indicesCount);

			inFile.read(reinterpret_cast<char*>(mesh.GetVertices().data()), verticesCount * sizeof(MeshVertex));

			inFile.read(reinterpret_cast<char*>(mesh.GetIndices().data()), indicesCount * sizeof(uint32_t));

			mesh.SetupMesh();

			mesh.GenerateRitterBoundingSphere();
		}

		GenerateRitterBoundingSphere();

		inFile.close();
	}

	void Model::SaveModel()
	{
		std::ofstream outFile("model.mesh", std::ios::binary);

		uint32_t meshCount = static_cast<uint32_t>(mMeshes.size());
		outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

		for (const Mesh& mesh : mMeshes) {
			// Write mesh header (vertexCount, indexCount, normalCount, texCoordCount)
			uint32_t verticesCount = mesh.GetVerticesCount();
			uint32_t indicesCount = mesh.GetIndicesCount();
			outFile.write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
			outFile.write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));

			// Write vertices
			outFile.write(reinterpret_cast<const char*>(mesh.GetVertices().data()), verticesCount * sizeof(Vertex));

			// Write indices
			outFile.write(reinterpret_cast<const char*>(mesh.GetIndices().data()), indicesCount * sizeof(uint32_t));
		}

		outFile.close();
	}

	void Model::GenerateRitterBoundingSphere()
	{
		if (mMeshes.empty()) return;

		// Start with the bounding sphere of the first mesh
		BoundingSphere modelSphere = mMeshes[0].GetBoundingSphere();

		for (size_t i = 1; i < mMeshes.size(); ++i)
		{
			const BoundingSphere& meshSphere = mMeshes[i].GetBoundingSphere();

			// Compute the distance between model sphere center and current mesh sphere center
			glm::vec3 direction = meshSphere.Center - modelSphere.Center;
			float distBetweenCenters = glm::length(direction);

			// Check if the mesh sphere is already within the current model sphere
			if (distBetweenCenters + meshSphere.Radius <= modelSphere.Radius)
				continue;

			// If the current model sphere fully encompasses the mesh sphere, skip
			if (distBetweenCenters + modelSphere.Radius <= meshSphere.Radius)
			{
				modelSphere.Center = meshSphere.Center;
				modelSphere.Radius = meshSphere.Radius;
				continue;
			}

			// Expand model sphere to include the mesh sphere
			float newRadius = (distBetweenCenters + modelSphere.Radius + meshSphere.Radius) / 2.0f;
			glm::vec3 offset = direction / distBetweenCenters * (newRadius - modelSphere.Radius);

			modelSphere.Center += offset;
			modelSphere.Radius = newRadius;
		}

		mBoundingSphere = modelSphere;
	}

	
}