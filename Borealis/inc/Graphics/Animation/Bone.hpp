/******************************************************************************/
/*!
\file		Bone.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class for Bone for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef BONE_HPP
#define BONE_HPP

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <string>

namespace Borealis 
{
	struct BoneData
	{
		//std::string name;
		int id;
		glm::mat4 offsetMatrix; // xform matrix for the bone
	};

	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class Bone
	{
	public:
		Bone() = default;
		Bone(std::string const& name, int id, std::vector<KeyPosition> const& pos,
		std::vector<KeyRotation> const& rot,
		std::vector<KeyScale> const& scale);

		void Update(float animationTime);

		glm::mat4 GetLocalTransform() { return mLocalTransform; }
		std::string GetBoneName() const { return mName; }
		int GetBoneID() { return mId; }
		int GetPositionIndex(float animationTime);
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);

		int mNumPositions;
		int mNumRotations;
		int mNumScalings;

		std::string mName;
		int mId;

		std::vector<KeyPosition> mPositions;
		std::vector<KeyRotation> mRotations;
		std::vector<KeyScale> mScales;

		glm::mat4 mLocalTransform;
	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);
		glm::mat4 InterpolateRotation(float animationTime);
		glm::mat4 InterpolateScaling(float animationTime);

	};
}
#endif