#include <BorealisPCH.hpp>
#include "Graphics/Animation/Animator.hpp"

namespace Borealis
{
	#define MAX_BONES 128

	void DecomposeMatrix(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
	{
		// Extract translation
		position = glm::vec3(matrix[3]);

		// Extract scale and rotation
		glm::mat3 rotScaleMatrix(matrix);
		scale.x = glm::length(rotScaleMatrix[0]);
		scale.y = glm::length(rotScaleMatrix[1]);
		scale.z = glm::length(rotScaleMatrix[2]);

		// Remove scale from rotation matrix
		if (scale.x != 0) rotScaleMatrix[0] /= scale.x;
		if (scale.y != 0) rotScaleMatrix[1] /= scale.y;
		if (scale.z != 0) rotScaleMatrix[2] /= scale.z;

		rotation = glm::quat_cast(rotScaleMatrix);
	}

	Animator::Animator() :
	mCurrentAnimation(nullptr), mLoop(true), mPlayed(false), mCurrentTime(0.f)
	{
		mFinalBoneMatrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
		{
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	Animator::Animator(Ref<Animation> animation) :
	mCurrentAnimation(animation), mLoop(true), mPlayed(false), mCurrentTime(0.f)
	{
		mFinalBoneMatrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
		{
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::UpdateAnimation(float dt)
	{
		if (mPlayed && mLoop)
		{
			mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->mDuration);
			mPlayed = false;
			mIsPlaying = true;
		}

		if (mCurrentAnimation && !mPlayed)
		{
			if (!mNextAnimation) // temp
			{
				mCurrentTime += mIsPlaying? /*mCurrentAnimation->mTicksPerSecond **/ dt * mSpeed : 0.f;

				if (mLoop)
				{
					mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->mDuration);
				}
				else if (mCurrentTime >= mCurrentAnimation->mDuration)
				{
					mCurrentTime = mCurrentAnimation->mDuration;
					mPlayed = true;
				}

				if (!mPlayed)
				{
					CalculateBoneTransform(&mCurrentAnimation->mRootNode, glm::mat4(1.0f));
				}
			} else
			{
				BlendTwoAnimations(mCurrentAnimation, mNextAnimation, mBlendFactor, dt);
			}
		}
	}

	bool Animator::HasAnimation()
	{
		return mCurrentAnimation ? true : false;
	}

	void Animator::PlayAnimation(Ref<Animation> animation)
	{
		mCurrentAnimation = animation;
		mCurrentTime = 0.0f;
		mPlayed = false;
		mIsPlaying = true;
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = mCurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(mCurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = mCurrentAnimation->mBoneDataMap;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offsetMatrix;
			mFinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}

	void Animator::CalculateBlendedBoneTransform(Ref<Animation> animationBase, AssimpNodeData const* node, Ref<Animation> animationLayer, AssimpNodeData const* nodeLayer, float currentTimeBase, float currentTimeLayer, glm::mat4 const& parentTransform, float blendFactor)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = animationBase->FindBone(nodeName);
		if (bone)
		{
			bone->Update(currentTimeBase);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 layerNodeTransform = nodeLayer->transformation;
		bone = animationLayer->FindBone(nodeName);
		if (bone)
		{
			bone->Update(currentTimeLayer);
			layerNodeTransform = bone->GetLocalTransform();
		}

		glm::quat rot0 = glm::quat_cast(nodeTransform);
		glm::quat rot1 = glm::quat_cast(layerNodeTransform);
		glm::quat finalRot = glm::slerp(rot0, rot1, blendFactor);
		glm::mat4 blendedMat = glm::mat4_cast(finalRot);
		blendedMat[3] = (1.f - blendFactor) * nodeTransform[3] + layerNodeTransform[3] * blendFactor;

		glm::mat4 globalTransformation = parentTransform * blendedMat;

		auto const& boneDataMap = animationBase->GetBoneDataMap();
		if (boneDataMap.find(nodeName) != boneDataMap.end())
		{
			int index = boneDataMap.at(nodeName).id;
			glm::mat4 offset = boneDataMap.at(nodeName).offsetMatrix;
			mFinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->children.size(); i++)
		{
			CalculateBlendedBoneTransform(animationBase, &node->children[i], 
				animationLayer, &nodeLayer->children[i], 
				currentTimeBase, currentTimeLayer, globalTransformation, blendFactor);
		}
	}

	void Animator::BlendTwoAnimations(Ref<Animation> baseAnimation, Ref<Animation> layerAnimation, float blendFactor, float dt)
	{
		float a = 1.f;
		float b = baseAnimation->GetDuration() / layerAnimation->GetDuration();
		float animSpeedMultiplierUp = (1.f - blendFactor) * a + b * blendFactor;

		a = layerAnimation->GetDuration() / baseAnimation->GetDuration();
		b = 1.f;
		float animSpeedMultiplierDown = (1.f - blendFactor) * a + b * blendFactor;

		mCurrentTimeBase += baseAnimation->GetTicksPerSecond() * dt * mSpeed * animSpeedMultiplierUp;
		mCurrentTimeLayer += layerAnimation->GetTicksPerSecond() * dt * mSpeed * animSpeedMultiplierDown;

		bool basePlayed = false;
		bool layerPlayed = false;

		if (mLoop)
		{
			mCurrentTimeBase = fmod(mCurrentTimeBase, baseAnimation->GetDuration());
		}
		else if (mCurrentTimeBase >= baseAnimation->GetDuration())
		{
			mCurrentTimeBase = baseAnimation->GetDuration();
			basePlayed = true;
		}

		if (mLoop)
		{
			mCurrentTimeLayer = fmod(mCurrentTimeLayer, layerAnimation->GetDuration());
		}
		else if (mCurrentTimeLayer >= layerAnimation->GetDuration())
		{
			mCurrentTimeLayer = layerAnimation->GetDuration();
			layerPlayed = true;
		}

		if (!(basePlayed && layerPlayed))
		{
			CalculateBlendedBoneTransform(baseAnimation, &baseAnimation->GetRootNode(),
				layerAnimation, &layerAnimation->GetRootNode(),
				mCurrentTimeBase, mCurrentTimeLayer, glm::mat4(1.f), blendFactor);
		}
	}

}