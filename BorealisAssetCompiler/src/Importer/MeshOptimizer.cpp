/******************************************************************************
/*!
\file       MeshOptimizer.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 23, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <iostream>
#include <meshoptimizer.h>

#include "Importer/MeshOptimizer.hpp"

namespace BorealisAssetCompiler
{
	void OptimizeModel(Model& model)
	{
		for (Mesh& mesh : model.mMeshes)
		{
			size_t indicesCount = mesh.mIndicesCount;
			size_t verticesCount = mesh.mVerticesCount;

			std::vector<unsigned int> remap(indicesCount);
			size_t optimizedVerticesCount = meshopt_generateVertexRemap(remap.data(),
																		mesh.mIndices.data(),
																		indicesCount,
																		mesh.mVertices.data(),
																		verticesCount,
																		sizeof(Vertex));

			std::vector<unsigned int> optimizedIndices;
			std::vector<Vertex> optimizedVertices;

			optimizedIndices.resize(indicesCount);
			optimizedVertices.resize(optimizedVerticesCount);

			//remove duplicate indices
			meshopt_remapIndexBuffer(optimizedIndices.data(), mesh.mIndices.data(), indicesCount, remap.data());

			meshopt_remapVertexBuffer(optimizedVertices.data(), mesh.mVertices.data(), verticesCount, sizeof(Vertex), remap.data());

			//improve locality of vertices
			meshopt_optimizeVertexCache(optimizedIndices.data(), optimizedIndices.data(), indicesCount, optimizedVerticesCount);

			//reduce pixel overdraw
			meshopt_optimizeOverdraw(optimizedIndices.data(), optimizedIndices.data(), indicesCount, &(optimizedVertices[0].Position.x), optimizedVerticesCount, sizeof(Vertex), 1.05f);

			//optimize access to vertex buffer
			meshopt_optimizeVertexFetch(optimizedVertices.data(), optimizedIndices.data(), indicesCount, optimizedVertices.data(), optimizedVerticesCount, sizeof(Vertex));

			//simplification

			//float threshold = 0.5f;
			//size_t targetIndicesCount = size_t(indicesCount * threshold);
			//float targetError = 0.2f;

			//std::vector<unsigned int> lodIndices(indicesCount);
			//float lodError = 0.f;
			//size_t lodIndicesCount = meshopt_simplify(&lodIndices[0],
			//													  optimizedIndices.data(),
			//													  indicesCount,
			//													  &optimizedVertices[0].Position.x,
			//													  optimizedVerticesCount,
			//													  sizeof(Vertex),
			//													  targetIndicesCount,
			//													  targetError,
			//													  0,
			//													  &lodError);
			//lodIndices.resize(lodIndicesCount);


			mesh.mIndices = optimizedIndices;
			mesh.mIndicesCount = (uint32_t)indicesCount;
			//mesh.mIndices = lodIndices;
			//mesh.mIndicesCount = lodIndicesCount;
			mesh.mVertices = optimizedVertices;
			mesh.mVerticesCount = (uint32_t)optimizedVerticesCount;
		}
	}

	void OptimizeModel(SkinnedModel& model)
	{
		for (SkinnedMesh& mesh : model.mMeshes)
		{
			size_t indicesCount = mesh.mIndicesCount;
			size_t verticesCount = mesh.mVerticesCount;

			std::vector<unsigned int> remap(indicesCount);
			size_t optimizedVerticesCount = meshopt_generateVertexRemap(remap.data(),
				mesh.mIndices.data(),
				indicesCount,
				mesh.mVertices.data(),
				verticesCount,
				sizeof(SkinnedVertex));

			std::vector<unsigned int> optimizedIndices;
			std::vector<SkinnedVertex> optimizedVertices;

			optimizedIndices.resize(indicesCount);
			optimizedVertices.resize(optimizedVerticesCount);

			//remove duplicate indices
			meshopt_remapIndexBuffer(optimizedIndices.data(), mesh.mIndices.data(), indicesCount, remap.data());

			meshopt_remapVertexBuffer(optimizedVertices.data(), mesh.mVertices.data(), verticesCount, sizeof(SkinnedVertex), remap.data());

			//improve locality of vertices
			meshopt_optimizeVertexCache(optimizedIndices.data(), optimizedIndices.data(), indicesCount, optimizedVerticesCount);

			//reduce pixel overdraw
			meshopt_optimizeOverdraw(optimizedIndices.data(), optimizedIndices.data(), indicesCount, &(optimizedVertices[0].Position.x), optimizedVerticesCount, sizeof(SkinnedVertex), 1.05f);

			//optimize access to vertex buffer
			meshopt_optimizeVertexFetch(optimizedVertices.data(), optimizedIndices.data(), indicesCount, optimizedVertices.data(), optimizedVerticesCount, sizeof(SkinnedVertex));

			//simplification

			//float threshold = 0.5f;
			//size_t targetIndicesCount = size_t(indicesCount * threshold);
			//float targetError = 0.2f;

			//std::vector<unsigned int> lodIndices(indicesCount);
			//float lodError = 0.f;
			//size_t lodIndicesCount = meshopt_simplify(&lodIndices[0],
			//													  optimizedIndices.data(),
			//													  indicesCount,
			//													  &optimizedVertices[0].Position.x,
			//													  optimizedVerticesCount,
			//													  sizeof(Vertex),
			//													  targetIndicesCount,
			//													  targetError,
			//													  0,
			//													  &lodError);
			//lodIndices.resize(lodIndicesCount);


			mesh.mIndices = optimizedIndices;
			mesh.mIndicesCount = (uint32_t)indicesCount;
			//mesh.mIndices = lodIndices;
			//mesh.mIndicesCount = lodIndicesCount;
			mesh.mVertices = optimizedVertices;
			mesh.mVerticesCount = (uint32_t)optimizedVerticesCount;
		}
	}
}

