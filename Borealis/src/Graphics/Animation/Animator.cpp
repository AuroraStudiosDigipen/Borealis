#include <BorealisPCH.hpp>
#include "Graphics/Animation/Animator.hpp"

namespace Borealis
{
	Animator::Animator()
	{
		mDeltaTime = {};
		mCurrentTime = 0.0;
		mCurrentAnimation = nullptr;

		mFinalBoneMatrices.reserve(300);

		for (int i = 0; i < 300; i++)
		{
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	Animator::Animator(Ref<Animation> animation)
	{
		mDeltaTime = {};
		mCurrentTime = 0.0;
		mCurrentAnimation = animation;

		mFinalBoneMatrices.reserve(300);

		for (int i = 0; i < 300; i++)
		{
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::UpdateAnimation(float dt)
	{
		mDeltaTime = dt;
		if (mCurrentAnimation)
		{
			mCurrentTime += mCurrentAnimation->mTicksPerSecond * dt;
			mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->mDuration);
			CalculateBoneTransform(&mCurrentAnimation->mRootNode, glm::mat4(1.0f));
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
}