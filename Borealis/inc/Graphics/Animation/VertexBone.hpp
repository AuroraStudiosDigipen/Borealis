/******************************************************************************/
/*!
\file		VertexBone.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class for Vertex Bone

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef VERTEXBONE_HPP
#define VERTEXBONE_HPP

#include <Core/Core.hpp>

#define MAX_BONE_INFLUENCE 4

namespace Borealis 
{
	struct VertexBoneData 
	{
		int32_t mBoneIds[MAX_BONE_INFLUENCE] = {};
		float mWeights[MAX_BONE_INFLUENCE] = {};

		VertexBoneData() {}

		void AddBoneData(int32_t boneid, float weight) {

			for (int32_t i{}; i < MAX_BONE_INFLUENCE; ++i)
			{
				if (mBoneIds[i] < 0)
				{
					mBoneIds[i] = boneid;
					mWeights[i] = weight;
					//printf("bone %d weight %f index %i\n", boneid, weight, i);
					break;
				}
			}
		}

		void ResetBoneData()
		{
			for (int i{}; i < MAX_BONE_INFLUENCE; ++i) 
			{
				mBoneIds[i] = -1;
				mWeights[i] = 0.f;
			}
		}
	};
}
#endif