#include <BorealisPCH.hpp>
#include <Core/LoggerSystem.hpp>
#include "Graphics/Animation/Bone.hpp"



namespace Borealis
{
	Bone::Bone(std::string const& name, int id, std::vector<KeyPosition> const& pos, std::vector<KeyRotation> const& rot, std::vector<KeyScale> const& scale)
		: mName(name), mId(id), mLocalTransform(1.f), mPositions(pos), mRotations(rot), mScales(scale)
	{
		mNumPositions = (int)pos.size();
		mNumRotations = (int)rot.size();
		mNumScalings =  (int)scale.size();
	}

	void Bone::Update(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScaling(animationTime);
		mLocalTransform = translation * rotation * scale;
	}

	int Bone::GetPositionIndex(float animationTime)
	{
		for (int i{}; i < mNumPositions - 1; ++i)
		{
			if (animationTime < mPositions[i + 1].timeStamp)
				return i;
		}
		BOREALIS_CORE_ASSERT(false, "Failed to find position");
		return -1;
	}

	int Bone::GetRotationIndex(float animationTime)
	{
		for (int i{}; i < mNumRotations - 1; ++i)
		{
			if (animationTime < mRotations[i + 1].timeStamp)
				return i;
		}
		BOREALIS_CORE_ASSERT(false, "Failed to find rotation");
		return -1;
	}

	int Bone::GetScaleIndex(float animationTime)
	{
		for (int i{}; i < mNumScalings - 1; ++i)
		{
			if (animationTime < mScales[i + 1].timeStamp)
				return i;
		}
		BOREALIS_CORE_ASSERT(false, "Failed to find scale");
		return -1;
	}

	float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 Bone::InterpolatePosition(float animationTime)
	{
		if (1 == mNumPositions)
			return glm::translate(glm::mat4(1.0f), mPositions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(mPositions[p0Index].timeStamp,
			mPositions[p1Index].timeStamp, animationTime);
		glm::vec3 finalPosition = glm::mix(mPositions[p0Index].position, mPositions[p1Index].position
			, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 Bone::InterpolateRotation(float animationTime)
	{
		if (1 == mNumRotations)
		{
			auto rotation = glm::normalize(mRotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(mRotations[p0Index].timeStamp,
			mRotations[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(mRotations[p0Index].orientation, mRotations[p1Index].orientation
			, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);

	}

	glm::mat4 Bone::InterpolateScaling(float animationTime)
	{
		if (1 == mNumScalings)
			return glm::scale(glm::mat4(1.0f), mScales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(mScales[p0Index].timeStamp,
			mScales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(mScales[p0Index].scale, mScales[p1Index].scale
			, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

}

