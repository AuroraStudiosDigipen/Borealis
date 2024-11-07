/******************************************************************************
/*!
\file       Frustum.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/Frustum.hpp>

#include <Graphics/Renderer2D.hpp>

namespace Borealis
{
	glm::vec4 NormalizePlane(glm::vec4 plane)
	{
		float length{ 1.f / std::sqrt(
			plane.x * plane.x +
			plane.y * plane.y +
			plane.z * plane.z) };

		plane = plane * length;
		return plane;
	}

	Frustum ComputeFrustum(glm::mat4 perspectiveMatrix)
	{
		//perspectiveMatrix = glm::transpose(perspectiveMatrix);

		glm::vec4 row0 = { perspectiveMatrix[0][0],perspectiveMatrix[1][0],perspectiveMatrix[2][0],perspectiveMatrix[3][0] };
		glm::vec4 row1 = { perspectiveMatrix[0][1],perspectiveMatrix[1][1],perspectiveMatrix[2][1],perspectiveMatrix[3][1] };
		glm::vec4 row2 = { perspectiveMatrix[0][2],perspectiveMatrix[1][2],perspectiveMatrix[2][2],perspectiveMatrix[3][2] };
		glm::vec4 row3 = { perspectiveMatrix[0][3],perspectiveMatrix[1][3],perspectiveMatrix[2][3],perspectiveMatrix[3][3] };

		Frustum frustum;

		frustum.leftPlane	= NormalizePlane(row3 + row0);   // Left Plane
		frustum.rightPlane	= NormalizePlane(row3 - row0);  // Right Plane
		frustum.bottomPlane = NormalizePlane(row3 + row1); // Bottom Plane
		frustum.topPlane	= NormalizePlane(row3 - row1);    // Top Plane
		frustum.nearPlane	= NormalizePlane(row3 + row2);   // Near Plane
		frustum.farPlane	= NormalizePlane(row3 - row2);

		return frustum;

		//Frustum frustum;

		//frustum.leftPlane	= NormalizePlane(perspectiveMatrix[3] + perspectiveMatrix[0]); // Left Plane
		//frustum.rightPlane	= NormalizePlane(perspectiveMatrix[3] - perspectiveMatrix[0]); // Right Plane
		//frustum.bottomPlane = NormalizePlane(perspectiveMatrix[3] + perspectiveMatrix[1]); // Bottom Plane
		//frustum.topPlane	= NormalizePlane(perspectiveMatrix[3] - perspectiveMatrix[1]); // Top Plane
		//frustum.nearPlane	= NormalizePlane(perspectiveMatrix[3] + perspectiveMatrix[2]); // Near Plane
		//frustum.farPlane	= NormalizePlane(perspectiveMatrix[3] - perspectiveMatrix[2]); // Far Plane

		//return frustum;
	}

	bool CullBoundingSphere(Frustum const& frustum, BoundingSphere boundingSphere)
	{
		glm::vec4 center = glm::vec4(boundingSphere.Center, 1.f);

		float leftD = glm::dot(frustum.leftPlane, center);
		if (leftD < -boundingSphere.Radius) return true;

		float rightD = glm::dot(frustum.rightPlane, center);
		if (rightD < -boundingSphere.Radius) return true;

		float topD = glm::dot(frustum.topPlane, center);
		if (topD < -boundingSphere.Radius) return true;

		float bottomD = glm::dot(frustum.bottomPlane, center);
		if (bottomD < -boundingSphere.Radius) return true;

		float nearD = glm::dot(frustum.nearPlane, center);
		if (nearD < -boundingSphere.Radius) return true;

		float farD = glm::dot(frustum.farPlane, center);
		if (farD < -boundingSphere.Radius) return true;

		////for clipping in the future
		// 
		//float leftD = glm::dot(frustum.leftPlane, center);
		//if (leftD > boundingSphere.Radius) return true;

		//float rightD = glm::dot(frustum.rightPlane, center);
		//if (rightD > boundingSphere.Radius) return true;

		//float topD = glm::dot(frustum.topPlane, center);
		//if (topD > boundingSphere.Radius) return true;

		//float bottomD = glm::dot(frustum.bottomPlane, center);
		//if (bottomD > boundingSphere.Radius) return true;

		//float nearD = glm::dot(frustum.nearPlane, center);
		//if (nearD > boundingSphere.Radius) return true;

		//float farD = glm::dot(frustum.farPlane, center);
		//if (farD > boundingSphere.Radius) return true;
		// 
		//if (leftD > -boundingSphere.Radius && leftD <= boundingSphere.Radius)
		//{
		//	*ptr_outcode |= GFX_CCLEFT;
		//}
		//if (rightD > -boundingSphere.Radius && rightD <= boundingSphere.Radius) 
		//{
		//	*ptr_outcode |= GFX_CCRIGHT;
		//}
		//if (bottomD > -boundingSphere.Radius && bottomD <= boundingSphere.Radius) 
		//{
		//	*ptr_outcode |= GFX_CCBOTTOM;
		//}
		//if (topD > -boundingSphere.Radius && topD <= boundingSphere.Radius) 
		//{
		//	*ptr_outcode |= GFX_CCTOP;
		//}
		//if (nearD > -boundingSphere.Radius && nearD <= boundingSphere.Radius) 
		//{
		//	*ptr_outcode |= GFX_CCNEAR;
		//}
		//if (farD > -boundingSphere.Radius && farD <= boundingSphere.Radius) 
		//{
		//	*ptr_outcode |= GFX_CCFAR;
		//}

		return false;
	}
}

