/******************************************************************************
/*!
\file       AssetStructs.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 23, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetStructs_HPP
#define AssetStructs_HPP

#include <glm/glm.hpp>

namespace BorealisAssetCompiler
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Mesh
	{
		std::vector<unsigned int> mIndices;
		std::vector<Vertex> mVertices;

		uint32_t mVerticesCount;
		uint32_t mIndicesCount;
	};

	struct Model
	{
	public:
		std::vector<Mesh> mMeshes;
	};
}

#endif
