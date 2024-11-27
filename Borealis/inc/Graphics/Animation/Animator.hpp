#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <Core/Core.hpp>

#include "Graphics/Model.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	class Animator
	{
	public:
		Animator();
		Animator(Ref<Animation> animation);

		void UpdateAnimation(float dt);
		bool HasAnimation();
		void PlayAnimation(Ref<Animation> animation);
		void BlendTwoAnimations(Ref<Animation> baseAnimation, Ref<Animation> layerAnimation, float blendFactor, float dt);

		std::vector<glm::mat4> GetFinalBoneMatrices() { return mFinalBoneMatrices; }
		void SetLoop(bool loop) { mLoop = loop; }
		void SetSpeed(float speed) { mSpeed = speed; }

		float mBlendFactor;
		Ref<Animation> mNextAnimation;

	private:
		void CalculateBoneTransform(AssimpNodeData const* node, glm::mat4 parentTransform);
		void CalculateBlendedBoneTransform(Ref<Animation> animationBase, AssimpNodeData const* node,
			Ref<Animation> animationLayer, AssimpNodeData const* nodeLayer,
			float currentTimeBase, float currentTimeLayer, glm::mat4 const& parentTransform, float blendFactor);

		std::vector<glm::mat4> mFinalBoneMatrices;
		Ref<Animation> mCurrentAnimation;

		bool mLoop;
		bool mPlayed;
		float mSpeed;
		float mCurrentTime;
		float mCurrentTimeLayer;
		float mCurrentTimeBase;
		//float mDeltaTime;
	};
}

#endif