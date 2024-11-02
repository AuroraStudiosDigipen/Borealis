/******************************************************************************
/*!
\file       Frustum.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef Frustum_HPP
#define Frustum_HPP

#include <glm/glm.hpp>

#include <Graphics/Model.hpp>

namespace Borealis
{
	struct Frustum
	{
		glm::vec4 topPlane;
		glm::vec4 bottomPlane;
		glm::vec4 leftPlane;
		glm::vec4 rightPlane;
		glm::vec4 nearPlane;
		glm::vec4 farPlane;
	};

	Frustum ComputeFrustum(glm::mat4 perspectiveMatrix);

	bool CullBoundingSphere(Frustum const& frustum, BoundingSphere boundingSphere);
}

#endif
