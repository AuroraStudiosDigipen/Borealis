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

	struct FrustumCorners
	{
		glm::vec3 nearTopLeft;
		glm::vec3 nearTopRight;
		glm::vec3 nearBottomLeft;
		glm::vec3 nearBottomRight;

		glm::vec3 farTopLeft;
		glm::vec3 farTopRight;
		glm::vec3 farBottomLeft;
		glm::vec3 farBottomRight;

		void Transform(glm::mat4 const& transformMtx)
		{
			nearTopLeft		= glm::vec3(transformMtx * glm::vec4(nearTopLeft	, 1.f));
			nearTopRight	= glm::vec3(transformMtx * glm::vec4(nearTopRight	, 1.f));
			nearBottomLeft	= glm::vec3(transformMtx * glm::vec4(nearBottomLeft	, 1.f));
			nearBottomRight = glm::vec3(transformMtx * glm::vec4(nearBottomRight, 1.f));

			farTopLeft		= glm::vec3(transformMtx * glm::vec4(farTopLeft		, 1.f));
			farTopRight		= glm::vec3(transformMtx * glm::vec4(farTopRight	, 1.f));
			farBottomLeft	= glm::vec3(transformMtx * glm::vec4(farBottomLeft	, 1.f));
			farBottomRight	= glm::vec3(transformMtx * glm::vec4(farBottomRight	, 1.f));
		}

		AABB GetAABB()
		{
			AABB aabb;

			// Initialize min and max to the first corner
			aabb.minExtent = nearTopLeft;
			aabb.maxExtent = nearTopLeft;

			// Array of all corners
			const glm::vec3 corners[] = {
				nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight,
				farTopLeft, farTopRight, farBottomLeft, farBottomRight
			};

			// Iterate through all corners to find min and max values
			for (const auto& corner : corners)
			{
				aabb.minExtent = glm::min(aabb.minExtent, corner);
				aabb.maxExtent = glm::max(aabb.maxExtent, corner);
			}

			return aabb;
		}
	};

	Frustum ComputeFrustum(glm::mat4 perspectiveMatrix);

	FrustumCorners GetCorners(Frustum const& frustum);

	void GetCornersViewProj(std::vector<glm::vec4>& corners, glm::mat4 const& viewProj);

	bool CullBoundingSphere(Frustum const& frustum, BoundingSphere boundingSphere);
}

#endif
